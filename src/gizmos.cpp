#include <vector>
#include <glm/glm.hpp>
#include "mesh.hpp"
#include "gizmos.hpp"
#include "globals.hpp"
#include "shader.hpp"

void Debug::DrawSphere(glm::vec3 pos, float radius, glm::vec4 col) {
    Mesh temp = IcoSphere3;
    temp.position = pos;
    temp.scale = glm::vec3(radius);
    temp.color = col;
    DrawMesh(temp, shader1.program);
}