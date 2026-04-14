#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "globals.hpp"
#include "init.hpp"
#include "input.hpp"
#include "shader.hpp"
#include "start.hpp"
#include "update.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

int framebufferWidth, framebufferHeight;
float currentFrame, lastFrame;

int main()
{
    window = InitGLFW(scr_width, scr_height);
    InitOpenGL();
    lastFrame = glfwGetTime();

    Start();
    glEnable(GL_DEPTH_TEST);

    shader5.Use();
    glUniform1i(glGetUniformLocation(shader5.program, "scene"), 0);
    glUniform1i(glGetUniformLocation(shader5.program, "bloomBlur"), 1);

    shader4.Use();
    glUniform1i(glGetUniformLocation(shader4.program, "image"), 0);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;

        ProcessInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Update();

        glfwSwapBuffers(window);
        lastFrame = currentFrame;
    }

    return 0;
}
