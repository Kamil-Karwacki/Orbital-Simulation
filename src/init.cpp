#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>
#include "init.hpp"
#include "globals.hpp"
#include "shader.hpp"
#include "input.hpp"
#include "math.hpp"

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
    shader1.Use();
}

void GlfwErrorCallback(int error, const char* description) {
    std::cout << "Error: " << error << "\n";
    std::cout << "Description: " << description << "\n";
}

void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
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
}