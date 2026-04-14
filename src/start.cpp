#include "start.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>

#include "globals.hpp"
#include "input.hpp"
#include "math.hpp"
#include "mesh.hpp"
#include "planet.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void Start()
{
    RotateCamera(1, 1);
    for (int i = 0; i < 512; i++)
    {
        p[i] = permutation[i % 256];
    }
    std::copy(std::begin(p), std::end(p), std::begin(oldP));
    IcoSphere3 = CreateIcoSphere(3);
    IcoSphere6 = CreateIcoSphere(6);

    for (auto &v : IcoSphere3.vertices)
    {
        v.color = glm::vec3(1.0f);
    }

    SetupMesh(IcoSphere3);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr1(-30000, 40000);

    for (int i = 0; i < sizeof(stars) / sizeof(Mesh); i++)
    {
        float x = distr1(gen);
        float y = distr1(gen);
        float z = distr1(gen);

        while (abs(x) + abs(y) + abs(z) < 90000)
        {
            x *= 1.01f;
            y *= 1.01f;
            z *= 1.01f;
        }

        while (abs(x) + abs(y) + abs(z) > 90000)
        {
            x *= 0.99f;
            y *= 0.99f;
            z *= 0.99f;
        }

        glm::vec3 pos = glm::vec3(x, y, z);

        stars[i] = IcoSphere3;
        stars[i].scale = glm::vec3(100);
        starsPos[i] = pos;
    }

    CreateBasicPlanets();

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
