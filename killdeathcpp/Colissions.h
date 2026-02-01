#ifndef COLISSIONS_H
#define COLISSIONS_H

#include "HitBox.h"
#include "tinyModel.h"

struct ObjectCollision {
    AABB modelSpaceAABB;
    AABB worldAABB;

    void updateModelAABBskins(tinyModel& model) {
        modelSpaceAABB.reset();
        for (Skin& skin : model.skins) {
            skin.jointMatrices.resize(skin.joints.size());
            for (size_t i = 0; i < skin.joints.size(); ++i) {
                glm::vec3 jointPos = glm::vec3(model.nodes[skin.joints[i]].globalMatrix[3]);
                modelSpaceAABB.expand(jointPos);
            }
        }
    }

    void updateModelAABBnodes(tinyModel& model)
    {
        modelSpaceAABB.reset();

        for (const Node& node : model.nodes) {
            if (node.glMeshIndex < 0)
                continue;
            modelSpaceAABB.expand(node.worldAABB);
        }
    }

    void updateWorldAABB(glm::mat4 modelMatrix) {
        worldAABB = computeWorldAABB(modelSpaceAABB, modelMatrix);
    }
};

enum class WorldShape {
    CapsuleWorld,
    SphereWorld,
    BoxWorld
};

struct WorldShapeEntry {
    WorldShape shape;
    int owner;
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


inline CapsuleWorldLoc computeCapsuleWorld(const ModelHitbox& hb, const glm::mat4& globalMatrix, const glm::mat4& localOffset) {
    glm::mat4 M = globalMatrix * localOffset;
    CapsuleWorldLoc c;
    c.p0 = glm::vec3(M * glm::vec4(0, hb.halfHeight, 0, 1));
    c.p1 = glm::vec3(M * glm::vec4(0, -hb.halfHeight, 0, 1));
    c.radius = hb.radius;
    return c;
}


inline bool point_Box_Colission(VerticeHitBox& box, glm::vec3 position, glm::vec3& Movement)
{
    glm::vec3 newPosition = position + Movement;
    float EPSILON = 0.000001f;
    box.close = false;
    if (newPosition.x < box.aabb.min.x) return false;
    if (newPosition.x > box.aabb.max.x) return false;
    if (newPosition.y < box.aabb.min.y) return false;
    if (newPosition.y > box.aabb.max.y) return false;
    if (newPosition.z < box.aabb.min.z) return false;
    if (newPosition.z > box.aabb.max.z) return false;
    box.close = true;
    CTriangle* cTriangle = box.cTriangles;
    for (int t = 0; t < box.cTrianglesCount; t++)
    {
        float Distance = glm::dot(cTriangle->Normal[0], newPosition) + cTriangle->D[0];
        cTriangle->close = Distance > 0.0f && Distance < RADIUS;
        if (cTriangle->close)
        {
            if (dot(cTriangle->Normal[1], newPosition) + cTriangle->D[1] < 0.0f)
            {
                if (dot(cTriangle->Normal[2], newPosition) + cTriangle->D[2] < 0.0f)
                {
                    if (dot(cTriangle->Normal[3], newPosition) + cTriangle->D[3] < 0.0f)
                    {
                        float offset = RADIUS - Distance;
                        if (offset < EPSILON) return false;
                        Movement += cTriangle->Normal[0] * offset;
                        return true;
                    }
                }
            }
        }

        cTriangle++;
    }

    if (!box.close) return false;
    CTriangle* Triangle = box.cTriangles;
    for (int t = 0; t < box.cTrianglesCount; t++)
    {
        if (Triangle->close)
        {
            for (int e = 0; e < 3; e++)
            {
                glm::vec3 VCP = newPosition - Triangle->Vertex[e];
                float EdotVCP = dot(Triangle->Edge[e], VCP);
                if (EdotVCP > 0.0f && EdotVCP < Triangle->EdgeLength[e])
                {
                    glm::vec3 Normal = VCP - Triangle->Edge[e] * EdotVCP;
                    float Distance = length(Normal);
                    if (Distance > 0.0f && Distance < RADIUS)
                    {
                        Movement += Normal * ((RADIUS) / Distance - 1.0f);
                        return true;
                    }
                }
            }
        }

        Triangle++;
    }

    if (box.close) return false;
    Triangle = box.cTriangles;
    for (int t = 0; t < box.cTrianglesCount; t++)
    {
        if (Triangle->close)
        {
            for (int v = 0; v < 3; v++)
            {
                glm::vec3 Normal = newPosition - Triangle->Vertex[v];
                float Distance = length(Normal);
                if (Distance > 0.0f && Distance < RADIUS)
                {
                    Movement += Normal * ((RADIUS) / Distance - 1.0f);
                    return true;
                }
            }
        }

        Triangle++;
    }
    return false;
}

inline bool AABBPointColission(AABB& box, glm::vec3 position) {
    if (position.x < box.min.x) return false;
    if (position.x > box.max.x) return false;
    if (position.y < box.min.y) return false;
    if (position.y > box.max.y) return false;
    if (position.z < box.min.z) return false;
    if (position.z > box.max.z) return false;
    return true;
}


inline std::vector<glm::vec3> computeVertexNormals(
    const std::vector<glm::vec3>& positions,
    const std::vector<unsigned int>& indices
) {
    std::vector<glm::vec3> normals(positions.size(), glm::vec3(0.0f));

    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];
        glm::vec3 v0 = positions[i0];
        glm::vec3 v1 = positions[i1];
        glm::vec3 v2 = positions[i2];
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));
        normals[i0] += faceNormal;
        normals[i1] += faceNormal;
        normals[i2] += faceNormal;
    }

    for (auto& n : normals) {
        n = glm::normalize(n);
    }
    return normals;
}




#endif