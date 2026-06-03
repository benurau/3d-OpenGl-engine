#pragma once
#include <glm/glm.hpp>


glm::vec3 CalculateDirection(glm::vec3& start, glm::vec3& endp) {
    glm::vec3 dir = endp - start;
    float len = glm::length(dir);
    if (len > 0.0001f)
    {
        dir /= len;

    }
    return dir;
}
