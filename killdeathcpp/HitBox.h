#ifndef HITBOX_H
#define HITBOX_H

#include "Geometry3D.h"
#include "misc.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <Mesh.h>



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

struct CapsuleWorldLoc {
    glm::vec3 p0;
    glm::vec3 p1;
    float radius;
};

struct CapsuleHitBoxWorld {
    CapsuleWorldLoc worldLoc;
    ModelHitbox hitBox;
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

inline CapsuleWorldLoc computeCapsuleWorld(const ModelHitbox& hb, const glm::mat4& globalMatrix, const glm::mat4& localOffset) {
    glm::mat4 M = globalMatrix * localOffset;
    CapsuleWorldLoc c;
    c.p0 = glm::vec3(M * glm::vec4(0, hb.halfHeight, 0, 1));
    c.p1 = glm::vec3(M * glm::vec4(0, -hb.halfHeight, 0, 1));
    c.radius = hb.radius;
    return c;
}

struct TriangleLocal
{
    glm::vec3 v0, v1, v2;
};

struct TriangleWorld
{
    glm::vec3 v0, v1, v2;
    glm::vec3 normal;
};

class VerticeHitBox
{
public:
    std::vector<TriangleLocal> localTriangles;
    std::vector<TriangleWorld> worldTriangles;
    AABB localAABB;
    AABB worldAABB;


    void buildFromMesh(const std::vector<Vertex>& vertices,const std::vector<GLuint>& indices){
        localTriangles.clear();
        worldTriangles.clear();
        localAABB.reset();
        for (size_t i = 0; i < indices.size(); i += 3)
        {
            TriangleLocal t;
            t.v0 = vertices[indices[i]].position;
            t.v1 = vertices[indices[i + 1]].position;
            t.v2 = vertices[indices[i + 2]].position;
            localTriangles.push_back(t);
            localAABB.expand(t.v0);
            localAABB.expand(t.v1);
            localAABB.expand(t.v2);
        }
        worldTriangles.resize(localTriangles.size());
    }

    void buildFromModel(std::vector<Mesh>& meshes)
    {
        localTriangles.clear();

        for (const Mesh& mesh : meshes)
        {
            std::vector<Vertex> vertices = mesh.vertices;
            std::vector<GLuint> indices = mesh.indices;

            for (size_t i = 0; i < indices.size(); i += 3)
            {
                TriangleLocal tri;
                tri.v0 = vertices[indices[i]].position;
                tri.v1 = vertices[indices[i + 1]].position;
                tri.v2 = vertices[indices[i + 2]].position;

                localTriangles.push_back(tri);
            }
        }
        worldTriangles.resize(localTriangles.size());
    }

    void updateWorld(const glm::mat4& modelMatrix)
    {
        worldAABB.reset();

        for (size_t i = 0; i < localTriangles.size(); i++)
        {
            const TriangleLocal& src = localTriangles[i];
            TriangleWorld& dst = worldTriangles[i];

            dst.v0 = glm::vec3(modelMatrix * glm::vec4(src.v0, 1.0f));
            dst.v1 = glm::vec3(modelMatrix * glm::vec4(src.v1, 1.0f));
            dst.v2 = glm::vec3(modelMatrix * glm::vec4(src.v2, 1.0f));

            dst.normal = glm::normalize(
                glm::cross(dst.v1 - dst.v0, dst.v2 - dst.v0)
            );

            worldAABB.expand(dst.v0);
            worldAABB.expand(dst.v1);
            worldAABB.expand(dst.v2);
        }
    }
};




#endif 