#pragma once
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};


struct Triangle {
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;

    Vertex aV;
    Vertex bV;
    Vertex cV;
};