#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Vertex{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

struct Mesh{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec4 color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    unsigned int VBO, VAO, EBO;
};

struct Noise {
    uint32_t seed;
    uint32_t octaves;
    uint32_t persistance;
    float strength;
};

void SetupMesh(Mesh& mesh_);
void DrawMesh(Mesh mesh, unsigned int shaderProgram);

Mesh CreateIcoSphere(unsigned int depth = 1);

inline Mesh IcoSphere3;
inline Mesh IcoSphere6;