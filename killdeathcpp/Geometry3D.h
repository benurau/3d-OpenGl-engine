#ifndef GEOMETRY3D_H
#define GEOMETRY3D_H

#include <glm/glm.hpp>
#include <string>



#define MAX_BONE_INFLUENCE 4
#define MAX_BONE_WEIGHTS 100


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec4 Tangent;
    glm::vec3 Bitangent;
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
    glm::uvec4 joints;
    glm::vec4 weights;

    Vertex()
        : position(0.0f), normal(0.0f), texCoords(0.0f),
        Tangent(0.0f), Bitangent(0.0f), joints(0.0f), weights(0.0f) {
        std::memset(m_BoneIDs, 0, sizeof(m_BoneIDs));
        std::memset(m_Weights, 0, sizeof(m_Weights));

    }

    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex)
        : position(pos), normal(norm), texCoords(tex),
        Tangent(0.0f), Bitangent(0.0f) {
        std::memset(m_BoneIDs, 0, sizeof(m_BoneIDs));
        std::memset(m_Weights, 0, sizeof(m_Weights));
    }

    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex,const glm::uvec4& Joints,const glm::vec4& Weights)
        : position(pos), normal(norm), texCoords(tex),
        Tangent(0.0f), Bitangent(0.0f), joints(Joints), weights(Weights) {
        std::memset(m_BoneIDs, 0, sizeof(m_BoneIDs));
        std::memset(m_Weights, 0, sizeof(m_Weights));
    }

    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex,
        const glm::vec4& tang, const glm::vec3& bitang,
        const int* boneIDs, const float* weights)
        : position(pos), normal(norm), texCoords(tex),
        Tangent(tang), Bitangent(bitang) {
        std::memcpy(m_BoneIDs, boneIDs, sizeof(m_BoneIDs));
        std::memcpy(m_Weights, weights, sizeof(m_Weights));
    }
};


struct Texture {
    unsigned int id = 0;
    std::string type;
    std::string path;
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