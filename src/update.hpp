#pragma once
#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>
#include <algorithm>
#include <iterator>

#include "init.hpp"
#include "input.hpp"
#include "globals.hpp"
#include "mesh.hpp"
#include "planet.hpp"
#include "gizmos.hpp"
#include "shader.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

ImVec2 mainManipulationWindowSize;

void DrawUI();

void Update() {
    shader1.Use();
    projection = glm::perspective(glm::radians(fov), (float)scr_width / (float)scr_height, 0.01f, 1000.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader1.program, "projection"), 1, GL_FALSE, &projection[0][0]);

    view = glm::translate(view, mainCam.pos);
    view = glm::lookAt(mainCam.pos, mainCam.pos + mainCam.front, glm::vec3(0,1,0));
    glUniformMatrix4fv(glGetUniformLocation(shader1.program, "view"), 1, GL_FALSE, &view[0][0]);
    glUniform3f(glGetUniformLocation(shader1.program, "viewPos"), mainCam.pos.x, mainCam.pos.y, mainCam.pos.z);


    for(int i=0; i<planets.size(); i++) { // current planet
        for(int j=0; j<planets.size(); j++) { // other planet
            UpdatePlanetsVel(planets[i], planets[j]);
        }
        planets[i].position += planets[i].vel * deltaTime * timeScale;
    }
    
    std::vector<Planet> previewPlanets;
    previewPlanets = planets;

    for(int i=0; i<previewPlanets.size(); i++) {
        previewPlanets[i].emissionStrength = 1.0f;
        if(previewPlanets[i].name == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad") {
            previewPlanets.erase(previewPlanets.begin() + i);
        }
    }

    shader2.Use();
    glUniformMatrix4fv(glGetUniformLocation(shader2.program, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader2.program, "view"), 1, GL_FALSE, &view[0][0]);
    for(int k=0; k<10; k++) {
        for(int i=0; i<previewPlanets.size(); i++) { // current planet
            for(int j=i; j<previewPlanets.size(); j++) { // other planet
                UpdatePlanetsVel2(previewPlanets[i], previewPlanets[j], previewTimeStep);
            }
            previewPlanets[i].position += previewPlanets[i].vel * previewTimeStep;
        
            for(int j=0; j<previewPlanets.size(); j++) { // Removes planet from preview when it starts intersecting with another planet
                if(previewPlanets[i].name == previewPlanets[j].name)
                    continue;
                
                if(glm::distance(previewPlanets[i].position, previewPlanets[j].position) < previewPlanets[j].scale.x) {
                    previewPlanets.erase(previewPlanets.begin() + i);
                }
            }
        }
        for(auto planet : previewPlanets) {
            Mesh temp{};
            temp = IcoSphere3;
            temp.position = planet.position;
            temp.color = glm::vec4(1.0f);
            temp.scale = glm::vec3(0.065f - (0.005f * k));
            DrawMesh(temp, shader2.program);
        }

    }

    // light calculations //
    shader1.Use();
    int n = 0;
    for(int i=0; i<planets.size(); i++) {
        if(planets[i].name == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad")
            continue;
        std::string num = std::to_string(n);
        if(planets[i].emissionStrength > 0.0f) {
            glUniform3f(glGetUniformLocation(shader1.program, std::string("light["+num+"].col").c_str()), planets[i].emissionCol.x, planets[i].emissionCol.y, planets[i].emissionCol.z);
            glUniform3f(glGetUniformLocation(shader1.program, std::string("light["+num+"].pos").c_str()), planets[i].position.x, planets[i].position.y, planets[i].position.z);
            glUniform1f(glGetUniformLocation(shader1.program, std::string("light["+num+"].constant").c_str()), 1.0f);
            glUniform1f(glGetUniformLocation(shader1.program, std::string("light["+num+"].linear").c_str()), 0.009f/planets[i].emissionStrength);
            glUniform1f(glGetUniformLocation(shader1.program, std::string("light["+num+"].quadratic").c_str()), 0.0032f/(planets[i].emissionStrength * 10));
            glUniform1f(glGetUniformLocation(shader1.program, std::string("light["+num+"].radius").c_str()), planets[i].scale.x);
            n++;
        }
        DrawPlanet(planets[i], shader1.program);
    }
    shader2.Use();
    DrawUI();
}

void DrawUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if(currentPlanet != nullptr) {
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
            ui_update.col[0] = currentPlanet->col.x;
            ui_update.col[1] = currentPlanet->col.y;
            ui_update.col[2] = currentPlanet->col.z;
            ui_update.col[3] = currentPlanet->col.w;
            ui_update.noises = currentPlanet->noises;
            ui_update.heights = currentPlanet->heights;
        }

        ImGui::SetNextWindowPos(ImVec2(5, mainManipulationWindowSize.y + 20), ImGuiCond_Once); 
        ImGui::Begin(currentPlanet->name.c_str());
        ImGui::SetWindowSize(ImVec2(300, scr_height - mainManipulationWindowSize.y - 100), ImGuiCond_Once);
        ImGui::Text("Mass");             ImGui::SameLine(); ImGui::InputFloat("##mass", &ui_update.mass);
        ImGui::Text("Velocity");         ImGui::SameLine(); ImGui::InputFloat3("##velocity", ui_update.vel);
        ImGui::Text("EmissionStrength"); ImGui::SameLine(); ImGui::InputFloat("##emissionstrength", &ui_update.emissionStrength);

        if(currentPlanet->emissionStrength > 0) {
            ImGui::Text("EmissionColor");    ImGui::ColorPicker4("##emissioncolor", ui_update.emissionCol);
        }

        if(ImGui::Button("Update")) {
            currentPlanet->vel = glm::vec3(ui_update.vel[0], ui_update.vel[1], ui_update.vel[2]);
            currentPlanet->mass = ui_update.mass;
            currentPlanet->emissionStrength = ui_update.emissionStrength;
            currentPlanet->emissionCol = glm::vec4(ui_update.emissionCol[0], ui_update.emissionCol[1], ui_update.emissionCol[2], ui_update.emissionCol[3]);
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

        currentPlanet->col = glm::vec4(ui_update.col[0], ui_update.col[1], ui_update.col[2], ui_update.col[3]);

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
            currentPlanet->mesh = CreateIcoSphere(currentPlanet->depth);
            for(auto& noise : currentPlanet->appliedNoise) {
                noise = glm::vec3(0);
            }
            for(int i=0; i<currentPlanet->noises.size(); i++) {
                ApplyNoise(*currentPlanet, currentPlanet->noises[i]);
            }
            ApplyNoise(*currentPlanet, ui_update.tempNoise);
            
            for(int i=0; i<currentPlanet->appliedNoise.size(); i++) {
                currentPlanet->mesh.vertices[i].position += currentPlanet->appliedNoise[i];
            }
            UpdateColors(*currentPlanet);
            ui_update.regeneratePlanet = false;
        }

        if(ImGui::Button("Delete")) {
            for(int i=0; i<planets.size(); i++) {
                if(planets[i].name == currentPlanet->name)
                    planets.erase(planets.begin() + i);
            }
        }
        ui_create.windowWidth = ImGui::GetWindowWidth();
        ImGui::End();

    } else {
        ui_update.firstRun = true;
    }


    // Main manipulation window // 
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
    ImGui::Text("PreviewTimescale"); ImGui::SameLine(); ImGui::InputFloat("##previeTimescale", &previewTimeStep);
    ImGui::Text(std::to_string(static_cast<int>(1.0f/deltaTime)).c_str());
    mainManipulationWindowSize = ImGui::GetWindowSize();
    ImGui::End();


    // Planet creation window // 
    if(ui_create.createPlanet) {
        currentPlanet = nullptr;
        ImGui::SetNextWindowPos(ImVec2(5, mainManipulationWindowSize.y + 20), ImGuiCond_Once); 
        ImGui::SetWindowSize(ImVec2(300, scr_height - mainManipulationWindowSize.y - 100), ImGuiCond_Once);
        ImGui::Begin("Create planet");
        ImGui::Text("Planets name");     ImGui::SameLine(); ImGui::InputText("##text", ui_create.buf, sizeof(ui_create.buf));
        ImGui::Text("Planets mass");     ImGui::SameLine(); ImGui::InputFloat("##mass", &ui_create.mass, 100, 1000);
        ImGui::Text("Planets radius");   ImGui::SameLine(); ImGui::InputFloat("##radius", &ui_create.radius, 0.1f, 1.0f);
        ImGui::Text("Planets position"); ImGui::SameLine(); ImGui::InputFloat3("##pos", ui_create.pos);
        ImGui::Text("Planets velocity"); ImGui::SameLine(); ImGui::InputFloat3("##vel", ui_create.vel);

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
                        ui_create.regenerateMesh = true;
                        continue;
                    }

                    bool hasInputted = ui_create.noises[i].octaves != ui_create_o.noises[i].octaves || ui_create.noises[i].persistance != ui_create_o.noises[i].persistance || ui_create.noises[i].seed != ui_create_o.noises[i].seed || ui_create.noises[i].strength != ui_create_o.noises[i].strength;
                    if(hasInputted) {
                        ui_create.regenerateMesh = true;
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
                        ui_create.regenerateMesh = true;
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
                    std::cout << "Planet with this name has already been created, use a different name\n";
                    ui_create.canCreatePlanet = false;
                    break;
                }
            }

            if(std::string(ui_create.buf).length() <= 0 || ui_create.mass <= 0 || ui_create.radius <= 0) {
                std::cout << "One of the parameters is wrong\n"; 
                ui_create.canCreatePlanet = false;
            }

            if(ui_create.canCreatePlanet) {
                std::cout << "Creating a planet " << glfwGetTime() << "\n";
                ui_create.createPlanet = false;
                ui_create.canCreatePlanet = false;
                PlanetData tempData{};
                tempData.name = std::string(ui_create.buf);
                tempData.mass = ui_create.mass;
                tempData.radius = ui_create.radius;
                tempData.pos = glm::vec3(ui_create.pos[0], ui_create.pos[1], ui_create.pos[2]);
                tempData.vel = glm::vec3(ui_create.vel[0], ui_create.vel[1], ui_create.vel[2]);
                tempData.depth = 40;
                Planet temp(tempData);
                planets.back().heights = ui_create.previewPlanet->heights;
                planets.back().appliedNoise = ui_create.previewPlanet->appliedNoise;
                planets.back().noises = ui_create.previewPlanet->noises;

                for(int i=0; i<planets.back().appliedNoise.size(); i++) {
                    planets.back().mesh.vertices[i].position += planets.back().appliedNoise[i];
                }

                UpdateColors(planets.back());
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

        if(ui_create.regenerateMesh) {
            ui_create.regenerateMesh = false;

            std::cout << "Regenerating preview planet's mesh " << glfwGetTime() << "\n";
            ui_create.previewPlanet->mesh = CreateIcoSphere(ui_create.previewPlanet->depth); // TODO
            for(auto& noise : ui_create.previewPlanet->appliedNoise) {
                noise = glm::vec3(0);
            }
            for(int i=0; i<ui_create.previewPlanet->noises.size(); i++) {
                ApplyNoise(*ui_create.previewPlanet, ui_create.previewPlanet->noises[i]);
            }
            
            for(int i=0; i<ui_create.previewPlanet->appliedNoise.size(); i++) {
                ui_create.previewPlanet->mesh.vertices[i].position += ui_create.previewPlanet->appliedNoise[i];
            }
            UpdateColors(*ui_create.previewPlanet);
        }

        glm::vec3 dir = glm::vec3(ui_create.vel[0], ui_create.vel[1], ui_create.vel[2]);
        glm::vec3 position = glm::vec3(ui_create.pos[0], ui_create.pos[1], ui_create.pos[2]);

        ui_create.previewPlanet->position = position;
        ui_create.previewPlanet->scale = glm::vec3(ui_create.radius);

        DrawPlanet(*ui_create.previewPlanet, shader1.program);
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
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if(ui_create.changedParams1) { // if user changed one of important parameters
        mainCam.pos = ui_create.previewPlanet->position + glm::vec3(ui_create.previewPlanet->scale.x * 2, 0, ui_create.previewPlanet->scale.x * 2);
        mainCam.front = glm::normalize(ui_create.previewPlanet->position - mainCam.pos);
        ui_create.changedParams1 = false;
    }
}