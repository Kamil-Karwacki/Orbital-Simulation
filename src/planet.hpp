#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "mesh.hpp"

struct PlanetData {
    std::string name;
    float mass;
    float radius = 1.0f;
    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec3 vel = glm::vec3(0.0f);
    glm::vec4 col = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 emissionCol;
    float emissionStrength = 0.0f;
    uint32_t seed = 0;
    uint32_t depth;
};

struct HeightCol {
    float height;
    float col[3];
};


class Planet {
public:
    Planet(PlanetData data);
    std::string name;
    Mesh mesh;
    float mass;
    float gravitationalConst = GRAVITATIONAL_CONST;
    float emissionStrength = 0.0f;
    glm::vec4 emissionCol = glm::vec4(1.0f);
    glm::vec3 vel = glm::vec3(0.0f);
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    std::vector<glm::vec3> lastPoints;
    glm::vec4 col;

    float noiseStrength = 0.2f;
    uint32_t seed;
    std::vector<Noise> noises;
    std::vector<HeightCol> heights;
    std::vector<glm::vec3> appliedNoise;
    uint32_t depth;
};


inline std::vector<Planet> planets;
inline Planet* currentPlanet;
void DrawPlanet(Planet planet, unsigned int shaderProgram);
void UpdatePlanetsVel(Planet &planet1, Planet &planet2);
void UpdatePlanetsVel2(Planet &planet1, Planet &planet2, float timeStep);
void SelectPlanet();
void UpdateColors(Planet &planet);
void ApplyNoise(Planet& planet, Noise noise);

struct UI_create {
    bool createPlanet = false, canCreatePlanet;
    char buf[255];
    float mass = 0, radius = 1;
    float pos[3], vel[3];
    float col[4];
    float windowWidth = 300;
    double seed = 0;
    float noiseStrength = 0.2f;
    uint32_t octaves = 2, persistance = 2;
    Planet* previewPlanet;
    bool regenerateMesh;
    bool changedParams1 = false;
    std::vector<Noise> noises;
    std::vector<HeightCol> heights;
};

struct UI_update {
    float mass;
    float vel[3];
    float emissionStrength;
    float emissionCol[4];
    float col[4];
    bool firstRun = true; // should planet data be updated into ui
    std::vector<Noise> noises;
    Noise tempNoise{}, lastTempNoise{};
    std::vector<HeightCol> heights;
    HeightCol tempHeightCol{};
    bool regeneratePlanet = false;
    float windowWidth = 200;
};

inline UI_create ui_create{};
inline UI_update ui_update{};
inline UI_create ui_create_o{};