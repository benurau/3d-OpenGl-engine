#ifndef COLISSIONS_H
#define COLISSIONS_H

#include "HitBox.h"
#include "tinyModel.h"

//enum class WorldShape {
//    CapsuleWorld,
//    SphereWorld,
//    BoxWorld
//};
//
//struct WorldShapeEntry {
//    WorldShape shape;
//    int owner;
//};



struct ShapeContact
{
    bool isColliding; 
    float penetrationDepth;
    glm::vec3 normal;
    glm::vec3 closestPoint;
};


struct ObjectCollision {
    AABB modelSpaceAABB;
    AABB worldAABB;
    std::vector<CapsuleHitBoxWorld> capsuleLocs;
    VerticeHitBox vHitbox;

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

    void updateWorldAABBV(glm::mat4 modelMatrix) {
        vHitbox.updateWorld(modelMatrix);
        worldAABB = computeWorldAABB(vHitbox.worldAABB, modelMatrix);
    }

    void updateWorldAABB(glm::mat4 modelMatrix) {
        worldAABB = computeWorldAABB(modelSpaceAABB, modelMatrix);
    }

    void updateCapsuleLocs(tinyModel& model, ObjectOrientation& orientation)
    {
        capsuleLocs.clear();
        capsuleLocs.reserve(model.hitboxes.size());
        for (const ModelHitbox& hb : model.hitboxes)
        {
            glm::mat4 boneWorld = orientation.modelMatrix * model.nodes[hb.node].globalMatrix;
            CapsuleWorldLoc worldCapsuleloc = computeCapsuleWorld(hb, boneWorld, hb.localOffset);
            CapsuleHitBoxWorld worldCapsule = { worldCapsuleloc, hb };
            capsuleLocs.push_back(worldCapsule);
        }
    }
};

inline glm::vec3 closestPointOnTriangle( const glm::vec3& p,const glm::vec3& a, const glm::vec3& b, const glm::vec3& c){
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 ap = p - a;
    float d1 = glm::dot(ab, ap);
    float d2 = glm::dot(ac, ap);
    if (d1 <= 0.0f && d2 <= 0.0f) return a;

    glm::vec3 bp = p - b;
    float d3 = glm::dot(ab, bp);
    float d4 = glm::dot(ac, bp);
    if (d3 >= 0.0f && d4 <= d3) return b;

    float vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
    {
        float v = d1 / (d1 - d3);
        return a + v * ab;
    }

    glm::vec3 cp = p - c;
    float d5 = glm::dot(ab, cp);
    float d6 = glm::dot(ac, cp);
    if (d6 >= 0.0f && d5 <= d6) return c;

    float vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
    {
        float w = d2 / (d2 - d6);
        return a + w * ac;
    }

    float va = d3 * d6 - d5 * d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
    {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return b + w * (c - b);
    }

    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;
    return a + ab * v + ac * w;
}

inline ShapeContact pointVertBoxCollision(const VerticeHitBox& box,const glm::vec3& point, float radius = 0.15f){
    ShapeContact contact{};
    contact.isColliding = false;
    contact.penetrationDepth = 0.0f;
    float deepestPenetration = 0.0f;
    glm::vec3 bestNormal(0.0f);
    glm::vec3 bestClosest(0.0f);

    for (const TriangleWorld& tri : box.worldTriangles)
    {
        glm::vec3 closest = closestPointOnTriangle(point,tri.v0, tri.v1, tri.v2);
        glm::vec3 delta = point - closest;
        float dist = glm::length(delta);
        if (dist < radius && dist > 0.000001f)
        {
            float penetration = radius - dist;
            if (penetration > deepestPenetration)
            {
                deepestPenetration = penetration;
                bestNormal = delta / dist;
                bestClosest = closest;
                contact.isColliding = true;
            }
        }
    }
    if (contact.isColliding)
    {
        contact.penetrationDepth = deepestPenetration;
        contact.normal = bestNormal;
        contact.closestPoint = bestClosest;
    }
    return contact;
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

inline ShapeContact pointInCapsule(const glm::vec3& point, CapsuleWorldLoc capsule){
    ShapeContact contact{};
    glm::vec3 ab = capsule.p1 - capsule.p0;
    glm::vec3 ap = point - capsule.p0;
    float abLenSq = glm::dot(ab, ab);
    float t = glm::dot(ap, ab) / abLenSq;
    t = glm::clamp(t, 0.0f, 1.0f);
    glm::vec3 closest = capsule.p0 + ab * t;
    glm::vec3 delta = point - closest;
    float dist = glm::length(delta);
    contact.closestPoint = closest;
    contact.penetrationDepth = capsule.radius - dist;
    contact.isColliding = contact.penetrationDepth > 0.0f;
    contact.normal = delta / dist;
    return contact;
}

inline std::vector<glm::vec3> computeVertexNormals( const std::vector<glm::vec3>& positions, const std::vector<unsigned int>& indices) {
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