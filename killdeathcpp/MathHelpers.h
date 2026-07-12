#pragma once
#include <glm/glm.hpp>
#include <cmath>

inline glm::vec3 CalculateDirection(glm::vec3& start, glm::vec3& endp) {
    glm::vec3 dir = endp - start;
    float len = glm::length(dir);
    if (len > 0.0001f)
    {
        dir /= len;

    }
    return dir;
}

inline float CalculateYawToTarget(const glm::vec3& from, const glm::vec3& to) {
    glm::vec3 dir = to - from;
    return glm::degrees(atan2(dir.x, dir.z));
}
