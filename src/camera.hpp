#pragma once
#include <glm/glm.hpp>

struct Camera {
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 rot;
    float speed = 5.0f;
    glm::vec3 front = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 orbitPos = glm::vec3(1.0f);
};