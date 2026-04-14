#pragma once
#include "globals.hpp"
#include "mesh.hpp"
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct PlanetData
{
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

struct HeightCol
{
    float height;
    float col[3];
};

class Planet
{
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
    uint32_t depth;

    uint32_t seed;
    std::vector<Noise> noises;
    std::vector<HeightCol> heights;
    std::vector<glm::vec3> appliedNoise;
};

inline std::vector<Planet> planets;
inline Planet *currentPlanet;
void DrawPlanet(Planet planet, unsigned int shaderProgram);
void UpdatePlanetsVel(Planet &planet1, Planet &planet2);
void UpdatePlanetsVel2(Planet &planet1, Planet &planet2, float timeStep);
void SelectPlanet();
void UpdateColors(Planet &planet);
void ApplyNoise(Planet &planet, Noise noise);
void CreateBasicPlanets();

struct UI_create
{
    bool createPlanet = false; // is user creating planet
    bool canCreatePlanet;      // can user create planet

    char buf[255];
    float mass = 0;
    float radius = 1;
    float pos[3];
    float vel[3];
    float col[4];

    double seed = 0;
    float noiseStrength = 0.2f;
    uint32_t octaves = 2, persistance = 2;
    Planet *previewPlanet;
    std::vector<Noise> noises;
    std::vector<HeightCol> heights;
    bool regeneratePlanet;       // should planets mesh be regenerated
    bool changedParams1 = false; // has user changed one of parameters

    float windowWidth = 300;
    unsigned int LOD;
    bool regenerateButton = false; // should show button to regenerate planet
};

struct UI_update
{
    float mass;
    float vel[3];
    float emissionStrength;
    float emissionCol[4];
    float col[4];
    float radius;

    std::vector<Noise> noises;
    Noise tempNoise{}, lastTempNoise{};
    std::vector<HeightCol> heights;
    HeightCol tempHeightCol{};

    bool firstRun = true;          // will planet data be updated into ui
    bool regeneratePlanet = false; // will planets mesh be regenerated
    float windowWidth = 200;
    unsigned int LOD, oldLOD;
    bool regenerateButton = false; // show button to regenerate planet
};

inline UI_create ui_create{};
inline UI_update ui_update{};
inline UI_create ui_create_o{};
