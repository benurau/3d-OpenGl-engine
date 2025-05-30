#ifndef GEOMETRY3D_H
#define GEOMETRY3D_H
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    Vertex()
        : position(0.0f), normal(0.0f), texCoords(0.0f) {
    }

    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex)
        : position(pos), normal(norm), texCoords(tex) {
    }
};


struct Triangle {
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;

    Vertex aV;
    Vertex bV;
    Vertex cV;

    bool close;
};

class CTriangle
{
public:
    glm::vec3 Vertex[3], Edge[3], Normal[4];
    float EdgeLength[3], D[4];
    bool close;
};

#endif