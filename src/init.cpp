#include <iostream>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <vector>
#include "init.hpp"
#include "globals.hpp"
#include "shader.hpp"
#include "input.hpp"
#include "planet.hpp"

GLFWwindow* InitGLFW(int screenWidth, int screenHeight) {
    glfwSetErrorCallback(GlfwErrorCallback);
    if(!glfwInit()) 
        throw std::runtime_error("Cannot initialize GLFW");
        
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Sim", NULL, NULL);
    if(!window)
        throw std::runtime_error("Couldn't create window!");
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, GLFWKeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);  
    glfwSetScrollCallback(window, ScrollCallback); 
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    
    return window;
}

void InitOpenGL() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");
    glEnable(GL_DEPTH_TEST);  
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glEnable(GL_CULL_FACE);  
    shader1.Init("./src/shaders/shader.vert", "./src/shaders/shader.frag");
    shader2.Init("./src/shaders/EmissionShader.vert", "./src/shaders/EmissionShader.frag");
    shader3.Init("./src/shaders/framebuffer.vert", "./src/shaders/framebuffer.frag");
    shader4.Init("./src/shaders/blur.vert", "./src/shaders/blur.frag");
    shader5.Init("./src/shaders/framebuffer.vert", "./src/shaders/framebuffer.frag");
    shader1.Use();

    GenerateFramebuffers();
    

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
    };
    // temp
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void GlfwErrorCallback(int error, const char* description) {
    std::cout << "Error: " << error << "\n";
    std::cout << "Description: " << description << "\n";
}

void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        if(currentPlanet)
            currentPlanet = nullptr;
        if(ui_create.createPlanet)
            ui_create.createPlanet = false;
    }
    
    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (key == GLFW_KEY_X && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        SelectPlanet();
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    scr_width = width;
    scr_height = height;
    if(scr_width <= 0)
        scr_width = 1;
    if(scr_height <= 0)
        scr_height = 1;

    GenerateFramebuffers();
}

void GenerateFramebuffers() {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(2, colorBuffers);
    for (int i=0; i<2; i++) {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scr_width, scr_height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, scr_width, scr_height);  
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer is not complete!\n";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (int i=0; i<2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scr_width, scr_height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!\n" ;
    }
}