#include <glad/glad.h>
#include <glm/glm.hpp>
#include "globals.hpp"
#include "mesh.hpp"
#include <vector>
#include <iostream>
#include "planet.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>
#include "math.hpp"

Planet::Planet(std::string _name, float _mass, Mesh _mesh, float _radius, glm::vec3 _position, glm::vec3 _vel, glm::vec4 _emissionCol, float _emissionStrength) {
    name = _name;
    mass = _mass;
    mesh = _mesh;
    position = _position;
    scale = glm::vec3(_radius);
    vel = _vel;
    emissionStrength = _emissionStrength;
    emissionCol = _emissionCol;
    planets.push_back(*this);
}

void DrawPlanet(Planet planet, unsigned int shaderProgram) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, planet.position);
    model = glm::scale(model, planet.scale);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform4f(glGetUniformLocation(shaderProgram, "aCol"), planet.mesh.color.x, planet.mesh.color.y, planet.mesh.color.z, planet.mesh.color.w);
    glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), planet.mesh.color.x, planet.mesh.color.y, planet.mesh.color.z);


    glBindVertexArray(planet.mesh.VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(planet.mesh.indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void UpdatePlanetsVel(Planet &planet1, Planet &planet2) {
    if(planet1.name == planet2.name) // if its the same planet
        return;
    glm::vec3 forceDir = planet1.position - planet2.position;
    glm::vec3 force = (glm::normalize(forceDir) * GRAVITATIONAL_CONST * planet1.mass * planet2.mass) / glm::length2(forceDir);
    glm::vec3 acceleration1 = -force / planet1.mass;
    glm::vec3 acceleration2 = force / planet2.mass;
    planet1.vel += acceleration1 * deltaTime * timeScale;
    planet2.vel += acceleration2 * deltaTime * timeScale;
}

void UpdatePlanetsVel2(Planet &planet1, Planet &planet2, float timeStep) {
    if(planet1.name == planet2.name) // if its the same planet
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