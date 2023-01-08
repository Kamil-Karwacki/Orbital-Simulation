#pragma once
#include <GLFW/glfw3.h>

GLFWwindow* InitGLFW(int screenWidth = 640, int screenHeight = 480);
void InitOpenGL();
void GenerateFramebuffers();
void GlfwErrorCallback(int error, const char* description);
void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

inline unsigned int fbo, rbo;
inline unsigned int colorBuffers[2];
inline unsigned int quadVAO, quadVBO;
inline unsigned int pingpongFBO[2];
inline unsigned int pingpongColorbuffers[2];