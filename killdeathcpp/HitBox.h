#ifndef HITBOX_H
#define HITBOX_H

#include "Geometry3D.h"
#include "misc.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>



enum class HitboxShape {
    Sphere,
    Capsule,
    Box
};

struct AABB {
    glm::vec3 min;
    glm::vec3 max;

    AABB() {
        min = glm::vec3(std::numeric_limits<float>::max());
        max = glm::vec3(std::numeric_limits<float>::lowest());
    }

    void reset()
    {
        min = glm::vec3(FLT_MAX);
        max = glm::vec3(-FLT_MAX);
    }

    void expand(const glm::vec3& p) {
        min = glm::min(min, p);
        max = glm::max(max, p);
    }

    void expand(const AABB& aabb)
    {
        expand(aabb.min);
        expand(aabb.max);
    }
};


struct ModelHitbox {
    HitboxShape shape;
    int node;
    glm::mat4 localOffset;
    float radius;
    float halfHeight;
};


inline AABB computeLocalAABB(const std::vector<Vertex>& vertices)
{
    AABB aabb;
    aabb.min = glm::vec3(std::numeric_limits<float>::max());
    aabb.max = glm::vec3(std::numeric_limits<float>::lowest());

    for (const auto& v : vertices) {
        aabb.min = glm::min(aabb.min, v.position);
        aabb.max = glm::max(aabb.max, v.position);
    }
    return aabb;
}

inline AABB computeWorldAABB(const AABB& local, const glm::mat4& model)
{
    AABB world;
    world.reset();
    glm::vec3 corners[8] = {
        {local.min.x, local.min.y, local.min.z},
        {local.max.x, local.min.y, local.min.z},
        {local.min.x, local.max.y, local.min.z},
        {local.max.x, local.max.y, local.min.z},
        {local.min.x, local.min.y, local.max.z},
        {local.max.x, local.min.y, local.max.z},
        {local.min.x, local.max.y, local.max.z},
        {local.max.x, local.max.y, local.max.z},
    };

    for (int i = 0; i < 8; ++i) {
        glm::vec3 p = glm::vec3(model * glm::vec4(corners[i], 1.0f));
        world.expand(p);
    }
    return world;
}


class VerticeHitBox {
public:
    AABB aabb;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    glm::mat3 NormalMatrix;
    glm::mat4 ModelMatrix;

    CTriangle* cTriangles;
    int cTrianglesCount;
    bool close;

    VerticeHitBox() {};
    VerticeHitBox(std::vector<Vertex> vertices, std::vector<GLuint> indices, glm::mat4 modelMat) {
        this->vertices = vertices;
        this->indices = indices;      
        this->cTrianglesCount = indices.size()/3;
        cTriangles = new CTriangle[cTrianglesCount];
        PrepareTriangles();
    }

    void Destroy()
    {
        delete[] cTriangles;
        SetDefaults();
    }

    void PrepareTriangles()
    {
        CTriangle *Triangle = cTriangles;
        int j = 0;
        for (int i = 0; i < cTrianglesCount; i++)
        {     
            for (int v = 0; v < 3; v++)
            {
                glm::vec3 Vertex = vertices[indices[j]].position;  
                //std::cout << vertices[indices[v]].position.x <<"  "<< vertices[indices[v]].position.y<<"   " << vertices[indices[v]].position.z << "\n";
                glm::vec3 transformed = glm::vec3(ModelMatrix * glm::vec4(Vertex, 1.0f));
                //std::cout << transformed.x <<"  "<< transformed.y<<"   " << transformed.z << "\n";
                Triangle->Vertex[v] = transformed;
                j++;
            }
            Triangle->Normal[0] = normalize(cross(Triangle->Vertex[1] - Triangle->Vertex[0], Triangle->Vertex[2] - Triangle->Vertex[0]));
            Triangle->D[0] = -dot(Triangle->Normal[0], Triangle->Vertex[0]);

            for (int e = 0; e < 3; e++)
            {
                Triangle->Edge[e] = Triangle->Vertex[(e + 1) % 3] - Triangle->Vertex[e];
                Triangle->EdgeLength[e] = length(Triangle->Edge[e]);
                Triangle->Edge[e] /= Triangle->EdgeLength[e];
                Triangle->Normal[1 + e] = cross(Triangle->Edge[e], Triangle->Normal[0]);
                Triangle->D[1 + e] = -dot(Triangle->Normal[1 + e], Triangle->Vertex[e]);
            }
            Triangle++;
        }
        if (cTrianglesCount == 0) return;
        aabb.min = aabb.max = cTriangles[0].Vertex[0];
        Triangle = cTriangles;

        for (int t = 0; t < cTrianglesCount; t++)
        {
            for (int v = 0; v < 3; v++)
            {
                if (Triangle->Vertex[v].x < aabb.min.x) aabb.min.x = Triangle->Vertex[v].x;
                if (Triangle->Vertex[v].x > aabb.max.x) aabb.max.x = Triangle->Vertex[v].x;
                if (Triangle->Vertex[v].y < aabb.min.y) aabb.min.y = Triangle->Vertex[v].y;
                if (Triangle->Vertex[v].y > aabb.max.y) aabb.max.y = Triangle->Vertex[v].y;
                if (Triangle->Vertex[v].z < aabb.min.z) aabb.min.z = Triangle->Vertex[v].z;
                if (Triangle->Vertex[v].z > aabb.max.z) aabb.max.z = Triangle->Vertex[v].z;
            }
            //std::cout << min.x<< "  " << min.y<< "  "<< max.x << "   " <<max.y<<"\n";
            Triangle++;
        }
        assert(cTrianglesCount * 3 <= indices.size());
        aabb.min -= RADIUS;
        aabb.max += RADIUS;
    }


    void updateModelMatrix(const glm::mat4x4& ModelMatrix)
    {
        this->ModelMatrix = ModelMatrix;
        this->NormalMatrix = glm::transpose(glm::inverse(glm::mat3x3(ModelMatrix)));
        PrepareTriangles();
    }


    void SetDefaults()
    {
        NormalMatrix = glm::mat3();
        ModelMatrix = glm::mat4();
        cTrianglesCount = 0;
        cTriangles = NULL;
    }

private:   
};




#endif 