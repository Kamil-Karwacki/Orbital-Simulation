#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>
#include <vector>

inline ImVec2 mainManipulationWindowSize;
inline uint16_t errorCode;
inline std::string errorText;

void DrawUI();
void CreatePlanet();
void ManipulationWindow();
void UpdatePlanet();
void DrawMenu();
void DrawMainMenu();

void ExportScene(std::string fileName);
void ImportScene(std::string fileName);

void ReloadSaves();

namespace Menu
{
inline char buf[255];
inline bool saveFile = false;
inline std::string exportPopupTxt;
inline uint8_t exportPopupCode;
inline bool shouldExport = false;

inline bool menu;
inline bool options;
inline bool exportScene = false;
inline bool loadScene = false;

inline bool loadFiles;
inline std::vector<std::string> files;
} // namespace Menu

namespace MainMenu
{
inline bool mainMenu = true;
inline bool loadScene = false;

} // namespace MainMenu
