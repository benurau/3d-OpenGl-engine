#pragma once
#include "tinyModel.h"
#include "Camera.h"



struct MeshObject {
    Mesh mesh;
    ObjectOrientation orientation;
    ObjectCollision colission;
};


// WARNING (#17): tinyModel is stored by value here. Copying a ModelObject copies the entire
// glTF scene (meshes, animations, skins, GPU materials). Consider using std::shared_ptr<tinyModel>
// or std::unique_ptr<tinyModel> if copies become a problem. At minimum, avoid accidental copies.
struct ModelObject {
    tinyModel model;
    ObjectOrientation orientation;
    ObjectCollision colission;

};




