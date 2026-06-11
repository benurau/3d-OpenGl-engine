#ifndef COLISSIONS_H
#define COLISSIONS_H

#include "HitBox.h"
#include "tinyModel.h"

struct ShapeContact {
    bool isColliding; 
    float penetrationDepth;
    glm::vec3 normal;
    glm::vec3 closestPoint;
};

struct ObjectCollision {
    AABB modelSpaceAABB;
    AABB worldAABB;
    bool hasVertices = false;
    bool hasCapsules = false;

    void setVerticeHitBox(const VerticeHitBox& vhb) {
        vHitbox = vhb;
        hasVertices = true;
    }

    const VerticeHitBox& getVerticeHitBox() const {
        return vHitbox;
    }

    void setCapsuleLocs(const std::vector<CapsuleHitBoxWorld>& locs) {
        capsuleLocs = locs;
        hasCapsules = !capsuleLocs.empty();
    }

    const std::vector<CapsuleHitBoxWorld>& getCapsuleLocs() const {
        return capsuleLocs;
    }

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

    void updateWorldAABBV(glm::mat4& modelMatrix) {
        vHitbox.updateWorld(modelMatrix);
        worldAABB = vHitbox.worldAABB;
        hasVertices = true;
    }

    void updateWorldAABB(glm::mat4& modelMatrix) {
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
        hasCapsules = !capsuleLocs.empty();
    }

private:
    std::vector<CapsuleHitBoxWorld> capsuleLocs;
    VerticeHitBox vHitbox;
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

inline ShapeContact pointVertBoxCollision(const VerticeHitBox& box, const glm::vec3& point, float radius = 0.15f) {
    ShapeContact contact{};
    contact.isColliding = false;
    contact.penetrationDepth = 0.0f;
    float deepestPenetration = 0.0f;
    glm::vec3 bestNormal(0.0f);
    glm::vec3 bestClosest(0.0f);
    for (const TriangleWorld& tri : box.worldTriangles) {
        glm::vec3 closest = closestPointOnTriangle(point, tri.v0, tri.v1, tri.v2);
        glm::vec3 delta = point - closest;
        float dist = glm::length(delta);
        if (dist < radius && dist > 0.000001f) {
            float penetration = radius - dist;
            if (penetration > deepestPenetration) {
                deepestPenetration = penetration;
                bestNormal = delta / dist;
                bestClosest = closest;
                contact.isColliding = true;
            }
        }
    }
    if (contact.isColliding) {
        contact.penetrationDepth = deepestPenetration;
        contact.normal = bestNormal;
        contact.closestPoint = bestClosest;
    }
    return contact;
}

inline ShapeContact AABBvsVertBox(const AABB& aabb, const VerticeHitBox& box) {
    ShapeContact contact{};
    contact.isColliding = false;
    contact.penetrationDepth = 0.0f;
    float deepestPenetration = 0.0f;
    glm::vec3 bestNormal(0.0f);
    glm::vec3 bestClosest(0.0f);
    glm::vec3 center = (aabb.min + aabb.max) * 0.5f;
    glm::vec3 halfExtents = (aabb.max - aabb.min) * 0.5f;
    for (const TriangleWorld& tri : box.worldTriangles) {
        glm::vec3 triClosest = closestPointOnTriangle(center, tri.v0, tri.v1, tri.v2);
        glm::vec3 delta = center - triClosest;
        float dist = glm::length(delta);
        if (dist < 0.000001f) continue;
        glm::vec3 dir = delta / dist;
        float effectiveRadius = glm::dot(glm::abs(dir), halfExtents);
        if (dist <= effectiveRadius) {
            float penetration = effectiveRadius - dist;
            if (penetration > deepestPenetration) {
                deepestPenetration = penetration;
                bestNormal = dir;
                bestClosest = triClosest;
                contact.isColliding = true;
            }
        }
    }
    if (contact.isColliding) {
        contact.penetrationDepth = deepestPenetration;
        contact.normal = bestNormal;
        contact.closestPoint = bestClosest;
    }
    return contact;
}

inline bool AABBPointColission(const AABB& box, const glm::vec3 position) {
    if (position.x < box.min.x) return false;
    if (position.x > box.max.x) return false;
    if (position.y < box.min.y) return false;
    if (position.y > box.max.y) return false;
    if (position.z < box.min.z) return false;
    if (position.z > box.max.z) return false;
    return true;
}

inline bool AABBvsAABB(const AABB& a, const AABB& b) {
    if (a.max.x < b.min.x) return false;
    if (a.min.x > b.max.x) return false;
    if (a.max.y < b.min.y) return false;
    if (a.min.y > b.max.y) return false;
    if (a.max.z < b.min.z) return false;
    if (a.min.z > b.max.z) return false;
    return true;
}

inline ShapeContact AABBvsAABBContact(const AABB& a, const AABB& b) {
    ShapeContact contact{};
    contact.isColliding = false;
    glm::vec3 overlap = glm::vec3(
        glm::min(a.max.x, b.max.x) - glm::max(a.min.x, b.min.x),
        glm::min(a.max.y, b.max.y) - glm::max(a.min.y, b.min.y),
        glm::min(a.max.z, b.max.z) - glm::max(a.min.z, b.min.z)
    );
    if (overlap.x >= 0.0f && overlap.y >= 0.0f && overlap.z >= 0.0f) {
        contact.isColliding = true;
        float minOverlap = glm::min(overlap.x, glm::min(overlap.y, overlap.z));
        glm::vec3 aCenter = (a.min + a.max) * 0.5f;
        glm::vec3 bCenter = (b.min + b.max) * 0.5f;
        glm::vec3 dir = bCenter - aCenter;
        if (overlap.x == minOverlap) {
            contact.normal = glm::vec3(dir.x > 0.0f ? 1.0f : -1.0f, 0.0f, 0.0f);
            contact.penetrationDepth = overlap.x;
        } else if (overlap.y == minOverlap) {
            contact.normal = glm::vec3(0.0f, dir.y > 0.0f ? 1.0f : -1.0f, 0.0f);
            contact.penetrationDepth = overlap.y;
        } else {
            contact.normal = glm::vec3(0.0f, 0.0f, dir.z > 0.0f ? 1.0f : -1.0f);
            contact.penetrationDepth = overlap.z;
        }
    }
    return contact;
}

inline ShapeContact pointInAABB(const glm::vec3& point, const AABB& box) {
    ShapeContact contact{};
    contact.isColliding = false;
    if (point.x >= box.min.x && point.x <= box.max.x &&
        point.y >= box.min.y && point.y <= box.max.y &&
        point.z >= box.min.z && point.z <= box.max.z) {
        contact.isColliding = true;
        glm::vec3 center = (box.min + box.max) * 0.5f;
        glm::vec3 halfExtents = (box.max - box.min) * 0.5f;
        glm::vec3 localPoint = point - center;
        glm::vec3 distToEdge = halfExtents - glm::abs(localPoint);
        float minDist = glm::min(distToEdge.x, glm::min(distToEdge.y, distToEdge.z));
        if (distToEdge.x == minDist) {
            contact.normal = glm::vec3(localPoint.x > 0.0f ? 1.0f : -1.0f, 0.0f, 0.0f);
            contact.penetrationDepth = halfExtents.x - glm::abs(localPoint.x);
        } else if (distToEdge.y == minDist) {
            contact.normal = glm::vec3(0.0f, localPoint.y > 0.0f ? 1.0f : -1.0f, 0.0f);
            contact.penetrationDepth = halfExtents.y - glm::abs(localPoint.y);
        } else {
            contact.normal = glm::vec3(0.0f, 0.0f, localPoint.z > 0.0f ? 1.0f : -1.0f);
            contact.penetrationDepth = halfExtents.z - glm::abs(localPoint.z);
        }
        contact.closestPoint = point - contact.normal * contact.penetrationDepth;
    }
    return contact;
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

inline bool isGrounded(ShapeContact& contact, float objectHeight) {
    if (contact.isColliding)
    {
        if (contact.normal.y > objectHeight)
        {
            return true;
        }
        return false;
    }
    return false;
}


#endif
