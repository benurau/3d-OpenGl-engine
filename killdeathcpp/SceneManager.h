#pragma once
#include "objects.h"
#include "Renderer.h"
#include "Material.h"

class SceneManager {
public:
    std::vector<MeshObject*> meshes;
    std::vector<ModelObject*> models;

    void Add(MeshObject& obj) { meshes.push_back(&obj); }
    void Add(ModelObject& obj) { models.push_back(&obj); }

    void Update(float dt)
    {
        for (auto* m : models) {
            m->model.updateAnimation(dt);
            m->model.updateNodeTransforms();
            m->model.updateSkins();
            m->colission.updateModelAABBskins(m->model);
            m->colission.updateWorldAABB(m->orientation.modelMatrix);
            m->colission.updateCapsuleLocs(m->model, m->orientation);
        }
    }

    void Render(Renderer& renderer, Material& mat, Camera& camera)
    {
        for (auto* m : meshes) {
            m->orientation.changeView(camera.GetViewMatrix());
            renderer.draw(m->mesh, m->orientation, mat);
        }
        for (auto* m : models) {
            m->orientation.changeView(camera.GetViewMatrix());
            renderer.drawModel(m->model, m->orientation);
        }
    }



};
