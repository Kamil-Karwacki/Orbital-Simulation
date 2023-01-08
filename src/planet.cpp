#include <glad/glad.h>
#include "globals.hpp"
#include <vector>
#include <iostream>
#include "math.hpp"
#include "planet.hpp"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

Planet::Planet(PlanetData data) {
    name = data.name;
    mass = data.mass;
    depth = data.depth;
    mesh = CreateIcoSphere(data.depth);
    for(int i=0; i<mesh.vertices.size(); i++) {
        this->appliedNoise.emplace_back(glm::vec3(0));
    }
    
    HeightCol tempHeightCol{};
    tempHeightCol.col[0] = data.col.x;
    tempHeightCol.col[1] = data.col.y;
    tempHeightCol.col[2] = data.col.z;
    tempHeightCol.height = 0;
    heights.emplace_back(tempHeightCol);
    
    position = data.pos;
    scale = glm::vec3(data.radius);
    vel = data.vel;
    emissionStrength = data.emissionStrength;
    emissionCol = data.emissionCol;
    seed = data.seed;
    UpdateColors(*this);
    planets.push_back(*this);
}

void DrawPlanet(Planet planet, unsigned int shaderProgram) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, planet.position);
    model = glm::scale(model, planet.scale);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);

    glBindVertexArray(planet.mesh.VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(planet.mesh.indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void UpdatePlanetsVel(Planet &planet1, Planet &planet2) {
    if(planet1.name == planet2.name || planet1.mass == 0 || planet2.mass == 0)
        return;
    glm::vec3 forceDir = planet1.position - planet2.position;
    glm::vec3 force = (glm::normalize(forceDir) * GRAVITATIONAL_CONST * planet1.mass * planet2.mass) / glm::length2(forceDir);
    glm::vec3 acceleration1 = -force / planet1.mass;
    glm::vec3 acceleration2 = force / planet2.mass;
    planet1.vel += acceleration1 * deltaTime * timeScale;
    planet2.vel += acceleration2 * deltaTime * timeScale;
}

void UpdatePlanetsVel2(Planet &planet1, Planet &planet2, float timeStep) {
    if(planet1.name == planet2.name || planet1.mass == 0 || planet2.mass == 0)
        return;
    glm::vec3 forceDir = planet1.position - planet2.position;
    glm::vec3 force = (glm::normalize(forceDir) * GRAVITATIONAL_CONST * planet1.mass * planet2.mass) / glm::length2(forceDir);
    glm::vec3 acceleration1 = -force / planet1.mass;
    glm::vec3 acceleration2 = force / planet2.mass;
    planet1.vel += acceleration1 * timeStep;
    planet2.vel += acceleration2 * timeStep;
}

void SelectPlanet() {
    MouseRaycast();
}

void UpdateColors(Planet &planet) {
    for(int i=0; i<planet.mesh.vertices.size(); i++) {
        for(int j=0; j<planet.heights.size(); j++) {
            if(glm::length(planet.mesh.vertices[i].position) > planet.heights[j].height) {
                planet.mesh.vertices[i].color = glm::vec3(planet.heights[j].col[0], planet.heights[j].col[1], planet.heights[j].col[2]);
            }
        }
    }

    glGenVertexArrays(1, &planet.mesh.VAO);
    glGenBuffers(1, &planet.mesh.VBO);
    glGenBuffers(1, &planet.mesh.EBO);

    glBindVertexArray(planet.mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, planet.mesh.VBO);

    glBufferData(GL_ARRAY_BUFFER, planet.mesh.vertices.size() * sizeof(Vertex), &planet.mesh.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet.mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, planet.mesh.indices.size() * sizeof(unsigned int), &planet.mesh.indices[0], GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::normal));

    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::color));

    glBindVertexArray(0);

}

void ApplyNoise(Planet& planet, Noise noise) {
    std::srand(noise.seed);
    std::random_shuffle(std::begin(p), std::end(p));
    if(noise.octaves <= 0)
        return;
    for(int i=0; i<planet.mesh.vertices.size(); i++) {
        glm::vec3 pointer = planet.mesh.vertices[i].position - glm::vec3(0);
        glm::vec3 pointer2 = planet.mesh.vertices[i].position - planet.scale;
        float noiseValue = OctavePerlin(abs(pointer2.x), abs(pointer2.y), abs(pointer2.z), noise.octaves, noise.persistance);
        planet.appliedNoise[i] += pointer * noiseValue * noise.strength;
    }
    std::copy(std::begin(oldP), std::end(oldP), std::begin(p));

    glGenVertexArrays(1, &planet.mesh.VAO);
    glGenBuffers(1, &planet.mesh.VBO);
    glGenBuffers(1, &planet.mesh.EBO);

    glBindVertexArray(planet.mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, planet.mesh.VBO);

    glBufferData(GL_ARRAY_BUFFER, planet.mesh.vertices.size() * sizeof(Vertex), &planet.mesh.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet.mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, planet.mesh.indices.size() * sizeof(unsigned int), &planet.mesh.indices[0], GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::normal));

    glBindVertexArray(0);
}