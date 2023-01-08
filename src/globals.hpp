#pragma once
#include "camera.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

inline unsigned int scr_width = 1280, scr_height = 720;
inline float deltaTime;
inline Camera mainCam;
inline const float GRAVITATIONAL_CONST = 6.67f / 1e11;
inline float timeScale = 1.0f, previewTimeStep = 80.0f;
inline GLFWwindow* window;
inline float distFromPlanet = 10;
inline glm::mat4 projection = glm::mat4(1.0f);
inline glm::mat4 view = glm::mat4(1.0f);
inline bool creatingPlanet = false;
inline float fov = 65;
inline bool bloom = true;
inline float exposure = 1.0f;
inline int bloomPasses = 20;