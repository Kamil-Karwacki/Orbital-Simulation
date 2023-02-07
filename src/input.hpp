#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void ProcessInput(GLFWwindow* window);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void RotateAroundPoint(glm::vec3 point, float xoffset, float yoffset);
void RotateCamera(double xoffset, double yoffset);

inline bool firstMouse = true;
inline float yaw   = 350.0f;
inline float pitch = 0.0f;
inline float lastX = 800.0f / 2.0;
inline float lastY = 600.0 / 2.0;
inline unsigned int cursorX, cursorY;