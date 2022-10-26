#pragma once
#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>
#include <algorithm>

#include "init.hpp"
#include "input.hpp"
#include "globals.hpp"
#include "mesh.hpp"
#include "planet.hpp"
#include "gizmos.hpp"
#include "shader.hpp"
#include "planet.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);

namespace ui_create
{
    bool createPlanet = false, canCreatePlanet;
    char buf[255];
    float mass = 0, radius = 0;
    float pos[3], vel[3];
    float col[4];
    float windowWidth = 0;
} 

namespace ui_update {
    float mass;
    float vel[3];
    float emissionStrength;
    float emissionCol[4];
    bool firstRun = true;
}

void DrawUI();

void Update() {
    shader1.Use();
    projection = glm::perspective(glm::radians(85.0f), (float)scr_width / (float)scr_height, 0.01f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader1.program, "projection"), 1, GL_FALSE, &projection[0][0]);

    view = glm::translate(view, mainCam.pos);
    view = glm::lookAt(mainCam.pos, mainCam.pos + mainCam.front, mainCam.up);
    glUniformMatrix4fv(glGetUniformLocation(shader1.program, "view"), 1, GL_FALSE, &view[0][0]);

    for(int i=0; i<planets.size(); i++) { // current planet
        for(int j=0; j<planets.size(); j++) { // other planet
            UpdatePlanetsVel(planets[i], planets[j]);
        }
        planets[i].position += planets[i].vel * deltaTime * timeScale;
        DrawPlanet(planets[i], shader1.program);
    }
    
    std::vector<Planet> previewPlanets;
    previewPlanets = planets;

    for(auto& p : previewPlanets) {
        p.emissionStrength = 1.0f;
    }
    glUniform3f(glGetUniformLocation(shader1.program, "viewPos"), mainCam.pos.x, mainCam.pos.y, mainCam.pos.z);

    shader2.Use();
    glUniformMatrix4fv(glGetUniformLocation(shader2.program, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader2.program, "view"), 1, GL_FALSE, &view[0][0]);
    for(int k=0; k<10; k++) {
        for(int i=0; i<previewPlanets.size(); i++) { // current planet
            for(int j=0; j<previewPlanets.size(); j++) { // other planet
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
        shader1.Use();

        //light calculations
        int n = 0;
        for(int i=0; i<planets.size(); i++) {
            if(planets[i].emissionStrength <= 0.0f)
                continue;
            std::string num = std::to_string(n);
            glUniform3f(glGetUniformLocation(shader1.program, std::string("light["+num+"].col").c_str()), planets[i].emissionCol.x, planets[i].emissionCol.y, planets[i].emissionCol.z);
            glUniform3f(glGetUniformLocation(shader1.program, std::string("light["+num+"].pos").c_str()), planets[i].position.x, planets[i].position.y, planets[i].position.z);
            glUniform1f(glGetUniformLocation(shader1.program, std::string("light["+num+"].constant").c_str()), 1.0f);
            glUniform1f(glGetUniformLocation(shader1.program, std::string("light["+num+"].linear").c_str()), 0.009f/planets[i].emissionStrength);
            glUniform1f(glGetUniformLocation(shader1.program, std::string("light["+num+"].quadratic").c_str()), 0.0032f/(planets[i].emissionStrength * 10));
            glUniform1f(glGetUniformLocation(shader1.program, std::string("light["+num+"].radius").c_str()), planets[i].scale.x);
            n++;
        }
        shader2.Use();
    }

    DrawUI();
}

void DrawUI() {
ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    if(currentPlanet != nullptr) {
        if(ui_update::firstRun) {
            ui_update::firstRun = false;
            ui_update::vel[0] = currentPlanet->vel.x;
            ui_update::vel[1] = currentPlanet->vel.y;
            ui_update::vel[2] = currentPlanet->vel.z;
            ui_update::mass = currentPlanet->mass;
            ui_update::emissionStrength = currentPlanet->emissionStrength;
            ui_update::emissionCol[0] = currentPlanet->emissionCol.x;
            ui_update::emissionCol[1] = currentPlanet->emissionCol.y;
            ui_update::emissionCol[2] = currentPlanet->emissionCol.z;
            ui_update::emissionCol[3] = currentPlanet->emissionCol.w;
        }

        ImGui::SetNextWindowPos(ImVec2(scr_width - ui_create::windowWidth - 5, 5), ImGuiCond_Always); 
        ImGui::Begin(currentPlanet->name.c_str());
        ImGui::Text("Mass"); ImGui::SameLine(); ImGui::InputFloat("##mass", &ui_update::mass, 100.0f, 1000.f);
        ImGui::Text("Velocity"); ImGui::SameLine(); ImGui::InputFloat3("##velocity", ui_update::vel);
        ImGui::Text("EmissionStrength"); ImGui::SameLine(); ImGui::InputFloat("##emissionstrength", &ui_update::emissionStrength);
        ImGui::Text("EmissionColor"); ImGui::SameLine(); ImGui::ColorPicker4("##emissioncolor", ui_update::emissionCol);
        ui_create::windowWidth = ImGui::GetWindowWidth();
        if(ImGui::Button("Update")) {
            currentPlanet->vel = glm::vec3(ui_update::vel[0], ui_update::vel[1], ui_update::vel[2]);
            currentPlanet->mass = ui_update::mass;
            currentPlanet->emissionStrength = ui_update::emissionStrength;
            currentPlanet->emissionCol = glm::vec4(ui_update::emissionCol[0], ui_update::emissionCol[1], ui_update::emissionCol[2], ui_update::emissionCol[3]);
            ui_update::firstRun = true;
        }
        ImGui::End();

        glm::vec3 dir = mainCam.pos - currentPlanet->position;
        dir = glm::normalize(dir);
        mainCam.pos = currentPlanet->position + glm::clamp(dir * distFromPlanet, currentPlanet->scale.x, FLT_MAX);
    } else {
        ui_update::firstRun = true;
    }

    // Main manipulation window // 
    ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiCond_Once); 
    ImGui::Begin("Planets");
    for(auto& p : planets) {
        if(ImGui::Button(p.name.c_str())) {
            currentPlanet = &p;
        }
    }
    if(ImGui::Button("Create planet")) {
        ui_create::createPlanet = true;
    }
    ImGui::Text("Timescale"); ImGui::SameLine(); ImGui::InputFloat("##timescale", &timeScale);
    ImGui::Text("PreviewTimescale"); ImGui::SameLine(); ImGui::InputFloat("##previeTimescale", &previewTimeStep);
    ImGui::End();

    if(ui_create::createPlanet) { // if user decides to create planet
        currentPlanet = nullptr;
        ImGui::Begin("Create planet");
        ImGui::Text("Planets name"); ImGui::SameLine(); ImGui::InputText("##text", ui_create::buf, sizeof(ui_create::buf));
        ImGui::Text("Planets mass"); ImGui::SameLine(); ImGui::InputFloat("##mass", &ui_create::mass, 100, 1000);
        ImGui::Text("Planets radius"); ImGui::SameLine(); ImGui::InputFloat("##radius", &ui_create::radius, 0.1f, 1.0f);
        ImGui::Text("Planets position"); ImGui::SameLine(); ImGui::InputFloat3("##pos", ui_create::pos);
        ImGui::Text("Planets velocity"); ImGui::SameLine(); ImGui::InputFloat3("##vel", ui_create::vel);
        ImGui::Text("Planets color"); ImGui::SameLine(); ImGui::ColorPicker4("##col", ui_create::col);

        if(ImGui::Button("Create planet")) {
            for(auto& p : planets) {
                if(p.name == ui_create::buf) {
                    std::cout << "Planet with this name has already been created, use a different name\n";
                    ui_create::canCreatePlanet = false;
                    break;
                }
            }

            if(std::string(ui_create::buf).length() <= 0 || ui_create::mass <= 0 || ui_create::radius <= 0) {
                std::cout << "One of the parameters is wrong\n"; 
                ui_create::canCreatePlanet = false;
            }

            if(ui_create::canCreatePlanet) {
                ui_create::createPlanet = false;
                PlanetSphere.color = glm::vec4(ui_create::col[0], ui_create::col[1], ui_create::col[2], ui_create::col[3]);
                Planet temp(std::string(ui_create::buf), ui_create::mass, PlanetSphere, ui_create::radius, glm::vec3(ui_create::pos[0], ui_create::pos[1], ui_create::pos[2]), glm::vec3(ui_create::vel[0], ui_create::vel[1], ui_create::vel[2]));
            } else {
                ui_create::canCreatePlanet = true;
            }
            
        }   

        ImGui::SameLine();
        if(ImGui::Button("Cancel")) {
            ui_create::createPlanet = false;
            ui_create::canCreatePlanet = true;
        }

        shader1.Use();

        glm::vec3 dir = glm::vec3(ui_create::vel[0], ui_create::vel[1], ui_create::vel[2]);
        glm::vec3 position = glm::vec3(ui_create::pos[0], ui_create::pos[1], ui_create::pos[2]);
        std::cout << position << std::endl;
        Mesh temp{};
        temp = IcoSphere6;
        temp.position = position;
        temp.color = glm::vec4(ui_create::col[0], ui_create::col[1], ui_create::col[2], ui_create::col[3]);
        temp.scale = glm::vec3(ui_create::radius);
        DrawMesh(temp, shader1.program);
        mainCam.pos = glm::vec3(position.x + ui_create::radius, position.y + ui_create::radius, position.z + ui_create::radius);
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
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}