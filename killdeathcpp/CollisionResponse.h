#pragma once
#include "Colissions.h"


inline glm::vec3 ResolveColissionPushBack(glm::vec3 movement, const ShapeContact& contact)
{
    if (!contact.isColliding)
        return movement;
    return movement += contact.normal * contact.penetrationDepth;
}



