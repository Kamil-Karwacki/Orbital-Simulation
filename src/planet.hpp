#pragma once
class Planet {
public:
    Planet(std::string _name, float _mass, Mesh _mesh, float _radius, glm::vec3 _position, glm::vec3 _vel = glm::vec3(0.0f), glm::vec4 _emissionCol = glm::vec4(1.0f), float _emissionStrength = 0.0f);
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
};

inline std::vector<Planet> planets;
inline Planet* currentPlanet;
void DrawPlanet(Planet planet, unsigned int shaderProgram);
void UpdatePlanetsVel(Planet &planet1, Planet &planet2);
void UpdatePlanetsVel2(Planet &planet1, Planet &planet2, float timeStep);
void SelectPlanet();
