#pragma one
#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "init.hpp"
#include "input.hpp"
#include "globals.hpp"
#include "mesh.hpp"
#include "planet.hpp"
#include "gizmos.hpp"
#include "math.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"


void Start() {
    for(int i=0; i<512; i++) {
        p[i] = permutation[i%256];
    }
    std::copy(std::begin(p), std::end(p), std::begin(oldP));
    IcoSphere3 = CreateIcoSphere(3);
    IcoSphere6 = CreateIcoSphere(6);

    PlanetData earthData{};
    earthData.name = "earth";
    earthData.mass = 81000;
    earthData.radius = 1.0f;
    earthData.pos = glm::vec3(0.0f);
    earthData.vel = glm::vec3(0.006f, 0.00f, 0.0f);
    earthData.col =  glm::vec4(0.0f, 0.7f, 0.0f, 1.0f);
    earthData.depth = 60;
    Planet earth(earthData);

    PlanetData sunData{};
    sunData.name = "sun";
    sunData.mass = 7 * 810000;
    sunData.radius = 3.0f;
    sunData.pos = glm::vec3(6.0f);
    sunData.vel = glm::vec3(0.0f);
    sunData.col = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    sunData.emissionCol = glm::vec4(1.0f);
    sunData.emissionStrength = 1000;
    sunData.depth = 40;
    Planet sun(sunData);

    PlanetData tempData{};
    tempData.name = "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";
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

    for(int i=0; i<planets.size(); i++) {
        std::cout << planets[i].name << "\n";
    }

    

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
}
