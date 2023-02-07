#include "ui.hpp"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <glm/gtx/io.hpp>
#include <filesystem>

#include "planet.hpp"
#include "shader.hpp"
#include "input.hpp"
#include <cstring>

void DrawUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if(errorCode) {
        ImGui::OpenPopup("Error");
    }
    if (ImGui::BeginPopupModal("Error")) {
        ImGui::Text(errorText.c_str());
        if(ImGui::Button("Close")) {
            errorCode = 0;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    /*if(Menu::menu && !MainMenu::mainMenu) {
        ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove); 
        ImGui::SetWindowPos(ImVec2(0,0));
        ImGui::SetWindowSize(ImVec2(scr_width, scr_height));
    }*/

    DrawMenu();
    if(Menu::menu && !MainMenu::mainMenu) {
        ImGui::End();
    }

    if(MainMenu::mainMenu) {
        DrawMainMenu();
    }


    if(!Menu::menu && !MainMenu::mainMenu) {
        UpdatePlanet();
        ManipulationWindow();
        CreatePlanet();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if(ui_create.changedParams1) { // if user changed one of important parameters
        mainCam.pos = ui_create.previewPlanet->position + glm::vec3(ui_create.previewPlanet->scale.x * 2, 0, ui_create.previewPlanet->scale.x * 2);
        mainCam.front = glm::normalize(ui_create.previewPlanet->position - mainCam.pos);
        ui_create.changedParams1 = false;
    }
}

void CreatePlanet() {
    if(!ui_create.createPlanet)
        return;

    currentPlanet = nullptr;
    ImGui::SetNextWindowPos(ImVec2(5, mainManipulationWindowSize.y + 20), ImGuiCond_Once); 
    ImGui::SetWindowSize(ImVec2(300, scr_height - mainManipulationWindowSize.y - 100), ImGuiCond_Once);
    ImGui::Begin("Create planet");
    ImGui::Text("Planets name");     ImGui::SameLine(); ImGui::InputText("##text", ui_create.buf, sizeof(ui_create.buf));
    ImGui::Text("Planets mass");     ImGui::SameLine(); ImGui::InputFloat("##mass", &ui_create.mass, 100, 1000);
    ImGui::Text("Planets radius");   ImGui::SameLine(); ImGui::InputFloat("##radius", &ui_create.radius, 0.1f, 1.0f);
    ImGui::Text("Planets position"); ImGui::SameLine(); ImGui::InputFloat3("##pos", ui_create.pos);
    ImGui::Text("Planets velocity"); ImGui::SameLine(); ImGui::InputFloat3("##vel", ui_create.vel);
    ImGui::Text("Level of detail");  ImGui::SameLine(); ImGui::InputScalar("##lod", ImGuiDataType_U32, &ui_create.LOD);
    if(ImGui::IsItemHovered())
        ImGui::SetTooltip("How detailed planet is");

    if(ui_create_o.LOD != ui_create.LOD)
        ui_create.regenerateButton = true;

    if(ui_create.regenerateButton && ImGui::Button("Regenerate mesh")) {
        ui_create.regeneratePlanet = true;
        ui_create.regenerateButton = false;
    }

    ImGui::NewLine();

    if(ImGui::Button("Add Noise")) { 
        Noise tempNoise{};
        tempNoise.octaves = 0;
        tempNoise.persistance = 0;
        tempNoise.seed = 0;
        tempNoise.strength = 0;
        ui_create.noises.emplace_back(tempNoise);
        ui_create.previewPlanet->noises.emplace_back(tempNoise);
    }

    ImGui::SameLine();
    if(ImGui::Button("Add Color")) { 
        HeightCol tempHeightCol{};
        ui_create.heights.emplace_back(tempHeightCol);
        ui_create.previewPlanet->heights.emplace_back(tempHeightCol);
    }   

    if(ImGui::CollapsingHeader("Noises")) {
        for(int i=0; i<ui_create.noises.size(); i++) {
            std::string hashtags = "##";
            if(ImGui::CollapsingHeader((std::string("Noise ") + std::to_string(i)).c_str())) {
                ImGui::Text("Seed");        ImGui::SameLine(); ImGui::DragScalar((hashtags+"seed"+std::to_string(i)).c_str(), ImGuiDataType_U32, &ui_create.noises[i].seed);
                ImGui::Text("Strength");    ImGui::SameLine(); ImGui::InputFloat((hashtags+"strength"+std::to_string(i)).c_str(), &ui_create.noises[i].strength, 0.1f);
                ImGui::Text("Octaves");     ImGui::SameLine(); ImGui::InputScalar((hashtags+"octaves"+std::to_string(i)).c_str(), ImGuiDataType_U32, &ui_create.noises[i].octaves);
                ImGui::Text("Persistance"); ImGui::SameLine(); ImGui::InputScalar((hashtags+"persistance"+std::to_string(i)).c_str(), ImGuiDataType_U32, &ui_create.noises[i].persistance);
                
                if(ImGui::Button(("Delete this noise ##" + std::to_string(i)).c_str())) {
                    ui_create.previewPlanet->noises.erase(ui_create.previewPlanet->noises.begin() + i);
                    ui_create.noises.erase(ui_create.noises.begin() + i);
                    ui_create.regeneratePlanet = true;
                    continue;
                }

                bool hasInputted = ui_create.noises[i].octaves != ui_create_o.noises[i].octaves || ui_create.noises[i].persistance != ui_create_o.noises[i].persistance || ui_create.noises[i].seed != ui_create_o.noises[i].seed || ui_create.noises[i].strength != ui_create_o.noises[i].strength;
                if(hasInputted) {
                    ui_create.regeneratePlanet = true;
                }

                ui_create.noises[i].octaves = std::clamp(static_cast<int>(ui_create.noises[i].octaves), 0, 100);
                ui_create.previewPlanet->noises[i].seed = ui_create.noises[i].seed;
                ui_create.previewPlanet->noises[i].octaves = ui_create.noises[i].octaves;
                ui_create.previewPlanet->noises[i].persistance = ui_create.noises[i].persistance;
                ui_create.previewPlanet->noises[i].strength = ui_create.noises[i].strength;
            }
        }
    }      

    if(ImGui::CollapsingHeader("Colors")) {
        for(int i=0; i<ui_create.heights.size(); i++) {
            std::string hashtags = "##";
            if(ImGui::CollapsingHeader((std::string("Color ") + std::to_string(i)).c_str())) {
                ImGui::Text("Height"); ImGui::SameLine(); ImGui::InputFloat((hashtags+"height"+std::to_string(i)).c_str(), &ui_create.heights[i].height);
                ImGui::Text("Color");  ImGui::SameLine(); ImGui::ColorPicker3((hashtags+"col"+std::to_string(i)).c_str(), ui_create.heights[i].col);
                
                bool hasInputted = ui_create.heights[i].height != ui_create_o.heights[i].height || (ui_create.heights[i].col[0] != ui_create_o.heights[i].col[0] && ui_create.heights[i].col[1] != ui_create_o.heights[i].col[1] && ui_create.heights[i].col[2] != ui_create_o.heights[i].col[2]);
                if(hasInputted) {
                    ui_create.regeneratePlanet = true;
                }

                ui_create.previewPlanet->heights[i].height = ui_create.heights[i].height;
                std::copy(std::begin(ui_create.heights[i].col), std::end(ui_create.heights[i].col), std::begin(ui_create.previewPlanet->heights[i].col));
            }
        }
    }     


    if(ui_create.pos[0] != ui_create_o.pos[0] || ui_create.pos[1] != ui_create_o.pos[1] || ui_create.pos[2] != ui_create_o.pos[2]) {
        ui_create.changedParams1 = true;
    }

    if(ImGui::Button("Create planet")) {
        for(auto& p : planets) {
            if(p.name == ui_create.buf) {
                errorText = "Planet with this name has already been created, use a different name";
                errorCode = 1;
                ui_create.canCreatePlanet = false;
                break;
            }
        }

        if(std::string(ui_create.buf).length() <= 0 || ui_create.mass <= 0 || ui_create.radius <= 0) {
            errorText = "One of the parameters is wrong, check if the radius or mass is not set to zero, or if a planet has a name";
            errorCode = 2;
            ui_create.canCreatePlanet = false;
        }

        if(ui_create.LOD <= 0) {
            errorText = "Planets level of detail can't be set to zero";
            errorCode = 2;
            ui_create.canCreatePlanet = false;
        }

        if(ui_create.canCreatePlanet) {
            std::cout << "Creating a planet " << ui_create.buf << "\n";
            ui_create.createPlanet = false;
            ui_create.canCreatePlanet = false;
            PlanetData tempData{};
            tempData.name = std::string(ui_create.buf);
            tempData.mass = ui_create.mass;
            tempData.radius = ui_create.radius;
            tempData.pos = glm::vec3(ui_create.pos[0], ui_create.pos[1], ui_create.pos[2]);
            tempData.vel = glm::vec3(ui_create.vel[0], ui_create.vel[1], ui_create.vel[2]);
            tempData.depth = ui_create.LOD;
            Planet temp(tempData);
            planets.back().heights = ui_create.previewPlanet->heights;
            planets.back().appliedNoise = ui_create.previewPlanet->appliedNoise;
            planets.back().noises = ui_create.previewPlanet->noises;

            for(int i=0; i<planets.back().mesh.vertices.size(); i++) {
                if(i > planets.back().appliedNoise.size())
                    break;
                planets.back().mesh.vertices[i].position += planets.back().appliedNoise[i];
            }

            UpdateColors(planets.back());

            planets.back().emissionCol = glm::vec4(1.0f);
            predictMesh.emplace_back(IcoSphere3);
        } else {
            ui_create.canCreatePlanet = true;
        }
    }   

    ImGui::SameLine();
    if(ImGui::Button("Cancel")) {
        ui_create.createPlanet = false;
        ui_create.canCreatePlanet = true;
    }

    shader1.Use();

    if(ui_create.regeneratePlanet) {
        ui_create.regeneratePlanet = false;

        ui_create.previewPlanet->mesh = CreateIcoSphere(ui_create.LOD);
        currentPlanet->appliedNoise.resize(currentPlanet->mesh.vertices.size(), glm::vec3(0));
        for(auto& noise : ui_create.previewPlanet->appliedNoise) {
            noise = glm::vec3(0);
        }
        for(int i=0; i<ui_create.previewPlanet->noises.size(); i++) {
            ApplyNoise(*ui_create.previewPlanet, ui_create.previewPlanet->noises[i]);
        }

        for(int i=0; i<ui_create.previewPlanet->mesh.vertices.size(); i++) {
            if(i > ui_create.previewPlanet->appliedNoise.size())
                break;
            ui_create.previewPlanet->mesh.vertices[i].position += ui_create.previewPlanet->appliedNoise[i];
        }
        UpdateColors(*ui_create.previewPlanet);
    }

    glm::vec3 dir = glm::vec3(ui_create.vel[0], ui_create.vel[1], ui_create.vel[2]);
    glm::vec3 position = glm::vec3(ui_create.pos[0], ui_create.pos[1], ui_create.pos[2]);

    ui_create.previewPlanet->position = position;
    ui_create.previewPlanet->scale = glm::vec3(ui_create.radius);

    shader2.Use();
    for(int i=0; i<10; i++) {
        dir = glm::normalize(dir);
        Mesh previewTemp{};
        previewTemp = IcoSphere3;
        previewTemp.position = position + dir * static_cast<float>(i);
        previewTemp.color = glm::vec4(1.0f);
        previewTemp.scale = glm::vec3(0.065f - (0.005f * i));
        DrawMesh(previewTemp, shader2.program);
    }
    ImGui::End();
    ui_create_o = ui_create;
}

void ManipulationWindow() {
    ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiCond_Once); 
    ImGui::Begin("Planets");
    for(auto& p : planets) {
        if(p.mass == 0)
            continue;

        if(ImGui::Button(p.name.c_str())) {
            currentPlanet = &p;
        }
    }
    if(ImGui::Button("Create planet")) {
        ui_create.createPlanet = true;
    }
    ImGui::Text("Timescale"); ImGui::SameLine(); ImGui::InputFloat("##timescale", &timeScale);
    if(ImGui::IsItemHovered())
        ImGui::SetTooltip("Speed at which simulation is running");

    ImGui::Text("PreviewTimescale"); ImGui::SameLine(); ImGui::InputFloat("##previewTimescale", &previewTimeStep);
    if(ImGui::IsItemHovered())
        ImGui::SetTooltip("Speed at which planet prediction is running");
        
    ImGui::Text("Prediciton depth"); ImGui::SameLine(); ImGui::InputScalar("##predictionDepth", ImGuiDataType_U32, &predictionDepth);
    if(ImGui::IsItemHovered())
        ImGui::SetTooltip("How far planet prediction draws into future");

    ImGui::Text(std::to_string(static_cast<int>(1.0f/deltaTime)).c_str());
    mainManipulationWindowSize = ImGui::GetWindowSize();
    ImGui::End();
}

void UpdatePlanet() {
    if(currentPlanet == nullptr) { 
        ui_update.firstRun = true; 
        ui_update.oldLOD = ui_update.LOD;
        return;
    }

    if(ui_update.firstRun) {
        ui_update.firstRun = false;
        ui_update.vel[0] = currentPlanet->vel.x;
        ui_update.vel[1] = currentPlanet->vel.y;
        ui_update.vel[2] = currentPlanet->vel.z;
        ui_update.mass = currentPlanet->mass;
        ui_update.emissionStrength = currentPlanet->emissionStrength;
        ui_update.emissionCol[0] = currentPlanet->emissionCol.x;
        ui_update.emissionCol[1] = currentPlanet->emissionCol.y;
        ui_update.emissionCol[2] = currentPlanet->emissionCol.z;
        ui_update.emissionCol[3] = currentPlanet->emissionCol.w;
        ui_update.noises = currentPlanet->noises;
        ui_update.heights = currentPlanet->heights;
        ui_update.radius = currentPlanet->scale.x;
        ui_update.LOD = currentPlanet->depth;
        ui_update.oldLOD = currentPlanet->depth;
        distFromPlanet = currentPlanet->scale.x * 3.0f;
    }
    RotateAroundPoint(currentPlanet->position, 0, 0);

    ImGui::SetNextWindowPos(ImVec2(5, mainManipulationWindowSize.y + 20), ImGuiCond_Once); 
    ImGui::Begin(currentPlanet->name.c_str());
    ImGui::SetWindowSize(ImVec2(300, scr_height - mainManipulationWindowSize.y - 100), ImGuiCond_Once);
    ImGui::Text("Mass");             ImGui::SameLine(); ImGui::InputFloat("##mass", &ui_update.mass);
    ImGui::Text("Velocity");         ImGui::SameLine(); ImGui::InputFloat3("##velocity", ui_update.vel);
    ImGui::Text("Emission Strength"); ImGui::SameLine(); ImGui::InputFloat("##emissionstrength", &ui_update.emissionStrength);
    ImGui::Text("Radius");           ImGui::SameLine(); ImGui::InputFloat("##radius", &ui_update.radius);
    ImGui::Text("Level of detail");  ImGui::SameLine(); ImGui::InputScalar("##lod", ImGuiDataType_U32, &ui_update.LOD);
    if(ImGui::IsItemHovered())
            ImGui::SetTooltip("How detailed planet is");

    if(ui_update.LOD != ui_update.oldLOD) 
        ui_update.regenerateButton = true;

    if(ui_update.regenerateButton) {
        if(ImGui::Button("Regenerate mesh")) {
            ui_update.regeneratePlanet = true;
            ui_update.regenerateButton = false;
        }
    }

    if(currentPlanet->emissionStrength > 0) {
        ImGui::Text("EmissionColor");   ImGui::ColorPicker4("##emissioncolor", ui_update.emissionCol);
    }

    if(ImGui::Button("Update")) {
        currentPlanet->vel = glm::vec3(ui_update.vel[0], ui_update.vel[1], ui_update.vel[2]);
        currentPlanet->mass = ui_update.mass;
        currentPlanet->emissionStrength = ui_update.emissionStrength;
        currentPlanet->emissionCol = glm::vec4(ui_update.emissionCol[0], ui_update.emissionCol[1], ui_update.emissionCol[2], ui_update.emissionCol[3]);
        currentPlanet->scale = glm::vec3(ui_update.radius);
        ui_update.firstRun = true;
    }

    ImGui::NewLine();
    if(ImGui::CollapsingHeader("Create noise")) {
        ImGui::Text("Seed");        ImGui::SameLine(); ImGui::DragScalar("##seedA", ImGuiDataType_U32, &ui_update.tempNoise.seed, 1);
        ImGui::Text("Octaves");     ImGui::SameLine(); ImGui::InputScalar("##octavesA", ImGuiDataType_U32, &ui_update.tempNoise.octaves);
        ImGui::Text("Persistance"); ImGui::SameLine(); ImGui::InputScalar("##persistanceA", ImGuiDataType_U32, &ui_update.tempNoise.persistance);
        ImGui::Text("Strength");    ImGui::SameLine(); ImGui::InputFloat("##strengthA", &ui_update.tempNoise.strength);
        ui_update.tempNoise.octaves = std::min(static_cast<int>(ui_update.tempNoise.octaves), 100);
        if(ImGui::Button("Add noise")) {
            currentPlanet->noises.emplace_back(ui_update.tempNoise);
            ui_update.tempNoise.octaves = 0;
            ui_update.tempNoise.persistance = 0;
            ui_update.tempNoise.seed = 0;
            ui_update.tempNoise.strength = 0;
            ui_update.firstRun = true;
        }
    }

    if(ImGui::CollapsingHeader("Add new color")) {
        ImGui::Text("Height"); ImGui::SameLine(); ImGui::InputFloat("##heightA", &ui_update.tempHeightCol.height);
        if(ImGui::IsItemHovered())
            ImGui::SetTooltip("The threshold after the color is applied");

        ImGui::Text("Color");  ImGui::SameLine(); ImGui::ColorPicker3("##colorA", ui_update.tempHeightCol.col);
        if(ImGui::Button("Add color")) {
            currentPlanet->heights.emplace_back(ui_update.tempHeightCol);
            ui_update.firstRun = true;
            ui_update.regeneratePlanet = true;
        }
    }

    ImGui::NewLine();
    if(ImGui::CollapsingHeader("Noises")) {
        for(int i=0; i<ui_update.noises.size(); i++) {
            std::string hashtags = "##";
            if(ImGui::CollapsingHeader((std::string("Noise ") + std::to_string(i)).c_str())) {
                ImGui::Text("Seed");        ImGui::SameLine(); ImGui::DragScalar((hashtags+"seed"+std::to_string(i)).c_str(), ImGuiDataType_U32, &ui_update.noises[i].seed);
                ImGui::Text("Strength");    ImGui::SameLine(); ImGui::InputFloat((hashtags+"strength"+std::to_string(i)).c_str(), &ui_update.noises[i].strength, 0.1f);
                ImGui::Text("Octaves");     ImGui::SameLine(); ImGui::InputScalar((hashtags+"octaves"+std::to_string(i)).c_str(), ImGuiDataType_U32, &ui_update.noises[i].octaves);
                ImGui::Text("Persistance"); ImGui::SameLine(); ImGui::InputScalar((hashtags+"persistance"+std::to_string(i)).c_str(), ImGuiDataType_U32, &ui_update.noises[i].persistance);
                
                if(ImGui::Button(("Delete this noise ##" + std::to_string(i)).c_str())) {
                    currentPlanet->noises.erase(currentPlanet->noises.begin() + i);
                    ui_update.noises.erase(ui_update.noises.begin() + i);
                    ui_update.regeneratePlanet = true;
                    continue;
                }

                bool hasInputted = currentPlanet->noises[i].octaves != ui_update.noises[i].octaves || currentPlanet->noises[i].persistance != ui_update.noises[i].persistance || currentPlanet->noises[i].seed != ui_update.noises[i].seed || currentPlanet->noises[i].strength != ui_update.noises[i].strength;
                if(hasInputted) {
                    ui_update.regeneratePlanet = true;
                }
                ui_update.noises[i].octaves = std::clamp(static_cast<int>(ui_update.noises[i].octaves), 0, 100);
                currentPlanet->noises[i].seed = ui_update.noises[i].seed;
                currentPlanet->noises[i].octaves = ui_update.noises[i].octaves;
                currentPlanet->noises[i].persistance = ui_update.noises[i].persistance;
                currentPlanet->noises[i].strength = ui_update.noises[i].strength;
            }
        }
    }

    ImGui::NewLine();

    if(ImGui::CollapsingHeader("Colors")) {
        for(int i=0; i<currentPlanet->heights.size(); i++) {
            std::string hashtags = "##";
            if(ImGui::CollapsingHeader((std::string("Color ") + std::to_string(i)).c_str())) {
                ImGui::Text("Height"); ImGui::SameLine(); ImGui::InputFloat((hashtags+"height"+std::to_string(i)).c_str(), &ui_update.heights[i].height);
                if(ImGui::IsItemHovered())
                    ImGui::SetTooltip("The threshold after which color is applied");

                ImGui::Text("Color");  ImGui::SameLine(); ImGui::ColorPicker3((hashtags+"col"+std::to_string(i)).c_str(), ui_update.heights[i].col);
                
                bool hasInputted = ui_update.heights[i].height != currentPlanet->heights[i].height || (ui_update.heights[i].col[0] != currentPlanet->heights[i].col[0] && ui_update.heights[i].col[1] != currentPlanet->heights[i].col[1] && ui_update.heights[i].col[2] != currentPlanet->heights[i].col[2]);
                if(hasInputted) {
                    ui_update.regeneratePlanet = true;
                }

                currentPlanet->heights[i].height = ui_update.heights[i].height;
                std::copy(std::begin(ui_update.heights[i].col), std::end(ui_update.heights[i].col), std::begin(currentPlanet->heights[i].col));
            }
        }
    }

    ui_update.windowWidth = ImGui::GetWindowWidth();

    // if one of the planet parameters is different, regenerate it //
    if((ui_update.tempNoise.octaves !=0 && ui_update.tempNoise.strength !=0) && (ui_update.tempNoise.persistance !=0 || ui_update.tempNoise.seed !=0)) {
        if(ui_update.tempNoise.octaves != ui_update.lastTempNoise.octaves ||
            ui_update.tempNoise.persistance != ui_update.lastTempNoise.persistance ||
            ui_update.tempNoise.seed != ui_update.lastTempNoise.seed ||
            ui_update.tempNoise.strength != ui_update.lastTempNoise.strength) {
            ui_update.regeneratePlanet = true;
            ui_update.lastTempNoise = ui_update.tempNoise;
        }
    }

    if(ui_update.regeneratePlanet) {
        currentPlanet->mesh = CreateIcoSphere(ui_update.LOD);
        currentPlanet->appliedNoise.resize(currentPlanet->mesh.vertices.size(), glm::vec3(0));
        for(auto& noise : currentPlanet->appliedNoise) {
            noise = glm::vec3(0);
        }
        for(int i=0; i<currentPlanet->noises.size(); i++) {
            ApplyNoise(*currentPlanet, currentPlanet->noises[i]);
        }
        ApplyNoise(*currentPlanet, ui_update.tempNoise);
        
        for(int i=0; i<currentPlanet->mesh.vertices.size(); i++) {
            currentPlanet->mesh.vertices[i].position += currentPlanet->appliedNoise[i];
        }
        UpdateColors(*currentPlanet);
        ui_update.regeneratePlanet = false;
    }

    if(ImGui::Button("Delete")) {
        for(int i=0; i<planets.size(); i++) {
            if(planets[i].name == currentPlanet->name) {
                planets.erase(planets.begin() + i);
                predictMesh.erase(predictMesh.begin() + i);
            }
        }
        currentPlanet = nullptr;
    }

    ImGui::SameLine();

    if(ImGui::Button("Cancel")) 
        currentPlanet = nullptr;
    

    ui_create.windowWidth = ImGui::GetWindowWidth();
    ImGui::End();
    ui_update.oldLOD = ui_update.LOD;
}

void DrawMenu() {
    if(!Menu::menu || MainMenu::mainMenu)
        return;

    ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove); 
    ImGui::SetWindowPos(ImVec2(0,0));
    ImGui::SetWindowSize(ImVec2(scr_width, scr_height));

    if(Menu::options) {
        ImGui::Checkbox("Bloom", &bloom);
        ImGui::PushItemWidth(scr_width/4);
        if(bloom) {
            ImGui::Text("Exposure"); ImGui::SameLine(); ImGui::InputFloat("##Exposure", &exposure, 0.1f, 1.0f);
            ImGui::Text("BloomPasses"); ImGui::SameLine(); ImGui::InputInt("##BloomPasses", &bloomPasses, 1, 10);
        }

        ImGui::SetCursorPosX(scr_width/2 - scr_width/8);
        if(ImGui::Button("Cancel", ImVec2(scr_width/4, 50.0f))) {
            Menu::options = false;
        }
        return;
    }

    if(Menu::exportScene) {
        ImVec2 textSize = ImGui::CalcTextSize("Filename");
        ImGui::SetCursorPosX((scr_width - (textSize.x))/2);
        ImGui::Text("Filename"); 
        
        ImGui::SetCursorPosX(scr_width/4);
        ImGui::PushItemWidth(scr_width/2);
        ImGui::InputText("##FileName", Menu::buf, sizeof(Menu::buf));

        if(ImGui::BeginPopupModal("ExportPopup")) {
            ImGui::Text(Menu::exportPopupTxt.c_str());

            if(Menu::exportPopupCode == 1) {
                if(ImGui::Button("Ok")) { 
                    ImGui::CloseCurrentPopup();
                }
            }

            if(Menu::exportPopupCode == 2) {
                if(ImGui::Button("Yes")) {
                    Menu::saveFile = true;
                    Menu::shouldExport = true;
                    ImGui::CloseCurrentPopup();
                }

                if(ImGui::Button("No")) {
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }

        if(Menu::shouldExport)
            ExportScene(std::string(Menu::buf));

        ImGui::SetCursorPosX(scr_width/2 - scr_width/8);
        if(ImGui::Button("Save", ImVec2(scr_width/4, 50.0f))) {
            ExportScene(std::string(Menu::buf));
        }

        ImGui::SetCursorPosX(scr_width/2 - scr_width/8);
        if(ImGui::Button("Cancel", ImVec2(scr_width/4, 50.0f))) {
            Menu::exportScene = false;
        }
        return;
    }

    if(Menu::loadScene) {
        for(int i=0; i<Menu::files.size(); i++) {
            ImGui::SetCursorPosX(scr_width/2 - scr_width/8);
            if(ImGui::Button(Menu::files[i].c_str(), ImVec2(scr_width/4, 50.0f))) 
                ImportScene(Menu::files[i] + ".scene");
            
            ImGui::SameLine();
            if(ImGui::Button(("Delete##"+std::to_string(i)).c_str(), ImVec2(50.0f, 50.0f))) { 
                std::filesystem::remove("scenes\\" + Menu::files[i] + ".scene");
                std::cout << "Deleting " << "scenes\\" + Menu::files[i] + ".scene\n";
                ReloadSaves();
            }
        }

        ImGui::NewLine();
        ImGui::SetCursorPosX(scr_width/2 - scr_width/8);
        if(ImGui::Button("Cancel", ImVec2(scr_width/4, 50.0f)))  {
            Menu::loadScene = false;
            std::cout << "Load scene\n";
        }
        
        return;
    }

    if(Menu::menu) {
        ImGui::SetCursorPosX(scr_width/2 - scr_width/8);
        ImGui::SetCursorPosY((scr_height/2) - 250.0f);
        if(ImGui::Button("Resume", ImVec2(scr_width/4, 50.0f))) {
            timeScale = oldTimeScale;
            Menu::menu = false;
            ImGui::End();
            return;
        }

        ImGui::SetCursorPosX(scr_width/2 - scr_width/8);
        if(ImGui::Button("Options", ImVec2(scr_width/4, 50.0f))) {
            Menu::options = true;
        }

        ImGui::SetCursorPosX(scr_width/2 - scr_width/8);
        if(ImGui::Button("Save scene", ImVec2(scr_width/4, 50.0f))) {
            Menu::exportScene = true;
        }

        ImGui::SetCursorPosX(scr_width/2 - scr_width/8);
        if(ImGui::Button("Load scene", ImVec2(scr_width/4, 50.0f)))  {
            ReloadSaves();
            Menu::loadScene = true;
        }

        ImGui::SetCursorPosX(scr_width/2 - scr_width/8);
        if(ImGui::Button("Exit", ImVec2(scr_width/4, 50.0f))) {
            glfwSetWindowShouldClose(window, 1);
        }
    }
}

void DrawMainMenu() {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f));
    ImGui::Begin("Main Menu", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove); 
    ImGui::PopStyleColor();
    ImGui::SetWindowPos(ImVec2(0,0));
    ImGui::SetWindowSize(ImVec2(scr_width, scr_height));

    if(MainMenu::loadScene) {
        for(int i=0; i<Menu::files.size(); i++) {
            ImGui::SetCursorPosX(scr_width/2 - scr_width/8);
            if(ImGui::Button(Menu::files[i].c_str(), ImVec2(scr_width/4, 50.0f))) {
                ImportScene(Menu::files[i] + ".scene");
                MainMenu::mainMenu = false;
                MainMenu::loadScene = false;
            }
            
            ImGui::SameLine();
            if(ImGui::Button(("Delete##"+std::to_string(i)).c_str(), ImVec2(50.0f, 50.0f))) { 
                std::filesystem::remove("scenes\\" + Menu::files[i] + ".scene");
                std::cout << "Deleting " << "scenes\\" + Menu::files[i] + ".scene\n";
                ReloadSaves();
            }
        }

        ImGui::NewLine();
        ImGui::SetCursorPosX(scr_width/2 - scr_width/8);
        if(ImGui::Button("Cancel", ImVec2(scr_width/4, 50.0f)))  {
            MainMenu::loadScene = false;
        }
        ImGui::End();
        return;
    }

    ImGui::SetCursorPosX(scr_width/4.0f - scr_width/8);
    ImGui::SetCursorPosY((scr_height/2) - 250.0f);
    if(ImGui::Button("Create new simulation", ImVec2(scr_width/4, 50.0f))) {
        std::cout << "Create new scene\n";
        planets.clear();
        CreateBasicPlanets();
        MainMenu::mainMenu = false;
        ImGui::End();
        return;
    }

    ImGui::SetCursorPosX(scr_width/4.0f - scr_width/8);
    if(ImGui::Button("Load scene", ImVec2(scr_width/4, 50.0f)))  {
        std::cout << "Loading scene\n";
        ReloadSaves();
        MainMenu::loadScene = true;
    }

    ImGui::SetCursorPosX(scr_width/4.0f - scr_width/8);
    if(ImGui::Button("Exit", ImVec2(scr_width/4, 50.0f))) {
        glfwSetWindowShouldClose(window, 1);
    }
    ImGui::End();
}

void ExportScene(std::string fileName) {
    if (!std::filesystem::is_directory("scenes") || !std::filesystem::exists("scenes")) {
        std::filesystem::create_directory("scenes");
    }

    std::ifstream f(std::string("scenes\\" + fileName + ".scene").c_str());
    if(f.good() && !Menu::saveFile) {
        ImGui::OpenPopup("ExportPopup");
        Menu::exportPopupTxt = "Do you want to overwrite file: " + fileName + ".scene";
        Menu::exportPopupCode = 2;
        Menu::saveFile = false;
    }

    if(!f.good())
        Menu::saveFile = true;

    if(!Menu::saveFile)
        return;

    unsigned int bufferSize = 0;
    uint16_t numOfPlanets = planets.size()-1;

    for(auto planet : planets) {
        if(planet.name == "")
            continue;
        std::cout << planet.name << "\n";
        bufferSize += sizeof(uint8_t) + planet.name.size() + 
        sizeof(planet.mass) + sizeof(planet.emissionStrength) + sizeof(planet.emissionCol) +
        sizeof(planet.vel) + sizeof(planet.position) + sizeof(planet.rotation) + sizeof(planet.scale) + 
        sizeof(planet.depth) + sizeof(planet.seed) + (planet.noises.size() * sizeof(Noise)) + (planet.heights.size() * sizeof(HeightCol)) + sizeof(uint32_t) * 2;
    }
    bufferSize += sizeof(numOfPlanets) + sizeof(oldTimeScale) + sizeof(previewTimeStep) + sizeof(predictionDepth);
    
    char* buffer = new char[bufferSize];

    unsigned int stride = 0;
    unsigned int beginning = 0;
    
    std::memcpy(buffer, &oldTimeScale, sizeof(oldTimeScale));
    beginning += sizeof(oldTimeScale);

    std::memcpy(buffer + beginning, &previewTimeStep, sizeof(previewTimeStep));
    beginning += sizeof(previewTimeStep);

    std::memcpy(buffer + beginning, &predictionDepth, sizeof(predictionDepth));
    beginning += sizeof(predictionDepth);

    std::memcpy(buffer + beginning, &numOfPlanets, sizeof(numOfPlanets));
    beginning += sizeof(numOfPlanets);


    for(auto planet : planets) {
        if(planet.name == "")
            continue;
        std::string name = planet.name;
        uint8_t nameLength = planet.name.length();
        float mass = planet.mass;
        float emission = planet.emissionStrength;
        glm::vec4 emissionCol = planet.emissionCol;

        glm::vec3 velocity = planet.vel;
        glm::vec3 position = planet.position;
        glm::vec3 rotation = planet.rotation;
        glm::vec3 scale = planet.scale;
        uint32_t depth = planet.depth;
        uint32_t seed = planet.seed;

        uint32_t heightsLength = planet.heights.size();
        uint32_t noisesLength = planet.noises.size();
        
        unsigned int modifier = 0;

        std::memcpy(buffer + beginning + stride + modifier, &nameLength, sizeof(nameLength));
        modifier += sizeof(nameLength);

        std::memcpy(buffer + beginning + stride + modifier, name.c_str(), name.size());
        modifier += name.size();

        std::memcpy(buffer + beginning + stride + modifier, &mass, sizeof(mass));
        modifier += sizeof(mass);

        std::memcpy(buffer + beginning + stride + modifier, &emission, sizeof(emission));
        modifier += sizeof(emission);

        std::memcpy(buffer + beginning + stride + modifier, &emissionCol, sizeof(emissionCol));
        modifier += sizeof(emissionCol);

        std::memcpy(buffer + beginning + stride + modifier, &velocity, sizeof(velocity));
        modifier += sizeof(velocity);

        std::memcpy(buffer + beginning + stride + modifier, &position, sizeof(position));
        modifier += sizeof(position);

        std::memcpy(buffer + beginning + stride + modifier, &rotation, sizeof(rotation));
        modifier += sizeof(rotation);

        std::memcpy(buffer + beginning + stride + modifier, &scale, sizeof(scale));
        modifier += sizeof(scale);

        std::memcpy(buffer + beginning + stride + modifier, &depth, sizeof(depth));
        modifier += sizeof(depth);

        std::memcpy(buffer + beginning + stride + modifier, &seed, sizeof(seed));
        modifier += sizeof(seed);

        std::memcpy(buffer + beginning + stride + modifier, &noisesLength, sizeof(noisesLength));
        modifier += sizeof(noisesLength);
        
        for(Noise& noise : planet.noises) {
            std::memcpy(buffer + beginning + stride + modifier, &noise, sizeof(noise));
            modifier += sizeof(noise);
        }

        std::memcpy(buffer + beginning + stride + modifier, &heightsLength, sizeof(heightsLength));
        modifier += sizeof(heightsLength);

        for(HeightCol& height : planet.heights) {
            std::memcpy(buffer + beginning + stride + modifier, &height, sizeof(height));
            modifier += sizeof(height);
        }

        stride += modifier;
    }


    std::ofstream file("scenes\\"+fileName+".scene", std::ios::binary);
    file.write(buffer, bufferSize);
    file.close();
    std::cout << "Wrote to file "  << std::filesystem::current_path().string() << "\\scenes\\" << fileName << ".scene " << bufferSize << " bytes\n\n";
    Menu::saveFile = false;
    Menu::shouldExport = false;
    Menu::exportPopupCode = 1;
    Menu::exportPopupTxt = "File sucssesfuly saved";
    ImGui::OpenPopup("ExportPopup");
    delete [] buffer;
}

void ReloadSaves() {
    Menu::files.clear();

    for (const auto & entry : std::filesystem::directory_iterator("./scenes")) {
        if(entry.path().extension() == ".scene") 
            Menu::files.emplace_back(entry.path().stem().string());
    }
}

void ImportScene(std::string fileName) {
    if (!std::filesystem::is_directory("scenes") || !std::filesystem::exists("scenes")) 
        std::filesystem::create_directory("scenes");
    

    std::ifstream file(std::string("scenes\\" + fileName).c_str(), std::ios::binary);
    file.seekg(0, std::ios::end);
	size_t length = file.tellg();
	file.seekg(0, std::ios::beg);
    std::cout << "Loading file " << fileName << ", size: " << length << " bytes\n";
	char* buffer = new char[length];

    float _timeScale;
    float previewTimeStep;
    unsigned int predictionDepth;
    uint16_t numOfPlanets;
    unsigned int beginning = 0;
    unsigned int stride = 0;

    file.read(buffer, length);

    std::memcpy(&_timeScale, buffer, sizeof(_timeScale));
    beginning += sizeof(_timeScale);

    std::memcpy(&previewTimeStep, buffer + beginning, sizeof(previewTimeStep));
    beginning += sizeof(previewTimeStep);

    std::memcpy(&predictionDepth, buffer + beginning, sizeof(predictionDepth));
    beginning += sizeof(predictionDepth);

    std::memcpy(&numOfPlanets, buffer + beginning, sizeof(numOfPlanets));
    beginning += sizeof(numOfPlanets);

    planets.clear();
    std::cout << "Number of planets: " << numOfPlanets << "\n";
    for(int i=0; i<numOfPlanets; i++) {
        uint8_t nameLength;
        float mass;
        float emission;
        glm::vec4 emissionCol;

        glm::vec3 velocity;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        uint32_t depth;
        uint32_t seed;
        std::vector<Noise> noises;
        std::vector<HeightCol> heights;

        uint32_t heightsLength;
        uint32_t noisesLength;

        unsigned int modifier = 0;

        std::memcpy(&nameLength, buffer + beginning + stride, sizeof(nameLength));
        std::string name(static_cast<int>(nameLength), ' ');
        modifier += sizeof(nameLength);

        std::memcpy(&name[0], buffer + beginning + stride + modifier, nameLength);
        modifier += (int)nameLength;

        std::memcpy(&mass, buffer + beginning + stride + modifier, sizeof(mass));
        modifier += sizeof(mass);

        std::memcpy(&emission, buffer + beginning + stride + modifier, sizeof(emission));
        modifier += sizeof(emission);

        std::memcpy(&emissionCol, buffer + beginning + stride + modifier, sizeof(emissionCol));
        modifier += sizeof(emissionCol);

        std::memcpy(&velocity, buffer + beginning + stride + modifier, sizeof(velocity));
        modifier += sizeof(velocity);

        std::memcpy(&position, buffer + beginning + stride + modifier, sizeof(position));
        modifier += sizeof(position);

        std::memcpy(&rotation, buffer + beginning + stride + modifier, sizeof(rotation));
        modifier += sizeof(rotation);

        std::memcpy(&scale, buffer + beginning + stride + modifier, sizeof(scale));
        modifier += sizeof(scale);

        std::memcpy(&depth, buffer + beginning + stride + modifier, sizeof(depth));
        modifier += sizeof(depth);

        std::memcpy(&seed, buffer + beginning + stride + modifier, sizeof(seed));
        modifier += sizeof(seed);

        std::memcpy(&noisesLength, buffer + beginning + stride + modifier, sizeof(noisesLength));
        modifier += sizeof(noisesLength);

        for(int i=0; i<noisesLength; i++) {
            Noise tempNoise{};
            std::memcpy(&tempNoise, buffer + beginning + stride + modifier, sizeof(tempNoise));
            noises.emplace_back(tempNoise);
            modifier += sizeof(tempNoise);
        }

        std::memcpy(&heightsLength, buffer + beginning + stride + modifier, sizeof(heightsLength));
        modifier += sizeof(heightsLength);

        for(int i=0; i<heightsLength; i++) {
            HeightCol tempHeight{};
            std::memcpy(&tempHeight, buffer + beginning + stride + modifier, sizeof(tempHeight));
            heights.emplace_back(tempHeight);
            modifier += sizeof(tempHeight);
        }

        stride += modifier;

        PlanetData planetData{};
        planetData.name = name;
        planetData.mass = mass;
        planetData.emissionStrength = emission;
        planetData.vel = velocity;
        planetData.pos = position;
        planetData.radius = scale.x;
        planetData.depth = depth;
        planetData.seed = seed;
        planetData.emissionCol = emissionCol;
        
        Planet temp(planetData);

        planets.back().heights = heights;

        for(int i=0; i<noises.size(); i++) {
            ApplyNoise(planets.back(), noises[i]);
        }
        
        for(int i=0; i<planets.back().mesh.vertices.size(); i++) {
            if(i >= planets.back().appliedNoise.size())
                break;
            planets.back().mesh.vertices[i].position += planets.back().appliedNoise[i];
        }
        UpdateColors(planets.back());
    }

    PlanetData tempData{};
    tempData.name = "";
    tempData.mass = 0;
    tempData.radius = 0;
    tempData.pos = glm::vec3(0.0f);
    tempData.vel = glm::vec3(0.0f);
    tempData.col = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    tempData.emissionCol = glm::vec4(1.0f);
    tempData.emissionStrength = 0;
    tempData.depth = 40;
    ui_create.previewPlanet = new Planet(tempData);
    ui_create.heights = ui_create.previewPlanet->heights;

    predictMesh.clear();
    for(int i=0; i<planets.size(); i++) {
        predictMesh.emplace_back(IcoSphere3);
    }

    file.close();
    delete [] buffer;
    Menu::loadScene = false;
}