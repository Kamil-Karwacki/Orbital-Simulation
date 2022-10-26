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

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void Start() {
    IcoSphere3 = CreateIcoSphere(3);
    IcoSphere6 = CreateIcoSphere(6);
    PlanetSphere = CreateIcoSphere(15);


    Mesh moonMesh = CreateIcoSphere(20);
    moonMesh.color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    moonMesh.position = glm::vec3(0.0f);

    Planet moon("moon", 10000, moonMesh, 0.1f, glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.001f, 0.007f));

    Mesh earthMesh = CreateIcoSphere(30);
    earthMesh.color = glm::vec4(0.0f, 0.7f, 0.0f, 1.0f);
    earthMesh.position = glm::vec3(0.0f);

    Planet earth("earth", 810000, earthMesh, 1.0f, glm::vec3(0.0f), glm::vec3(0.006f, 0.00f, 0.0f));

    Mesh sunMesh = CreateIcoSphere(50);
    sunMesh.color = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);
    sunMesh.position = glm::vec3(0.0f);

    Planet sun("sun", 3 * 810000, sunMesh, 3.0f, glm::vec3(6.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(1.0f), 1000);

    for(int i=0; i<planets.size(); i++) {
        std::cout << planets[i].name << "\n";
    }

    timeScale = 10.0f;

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
