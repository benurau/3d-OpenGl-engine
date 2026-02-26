#pragma once
#include "tinyModel.h"



struct MeshObject {
    Mesh mesh;
    ObjectOrientation orientation;
    ObjectCollision colission;
};


struct ModelObject {
    tinyModel& model;
    ObjectOrientation orientation;
    ObjectCollision colission;
};


