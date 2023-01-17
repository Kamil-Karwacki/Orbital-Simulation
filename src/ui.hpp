#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <string>

inline ImVec2 mainManipulationWindowSize;
inline uint16_t errorCode;
inline std::string errorText;

void DrawUI();
void CreatePlanet();
void ManipulationWindow();
void UpdatePlanet();
