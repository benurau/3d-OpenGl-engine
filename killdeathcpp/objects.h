#pragma once
#include "tinyModel.h"



struct MeshObject {
    Mesh mesh;
    ObjectOrientation orientation;
    VerticeHitBox hitbox;
};


struct ModelObject {
    tinyModel& model;
    ObjectOrientation orientation;
    ObjectCollision colission;

};


