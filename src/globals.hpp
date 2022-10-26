#pragma once
#include "camera.hpp"
#include <GLFW/glfw3.h>

inline unsigned int scr_width = 1280, scr_height = 720;
inline float deltaTime;
inline Camera mainCam;
inline const float GRAVITATIONAL_CONST = 6.67f / 1e11;
inline float timeScale = 1.0f, previewTimeStep = 80.0f;
inline GLFWwindow* window;
inline float distFromPlanet;