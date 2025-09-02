#ifndef MESH_H
#define MESH_H

#include "ObjectOrientation.h"
#include "Material.h"


class Mesh {
public:
    GLuint vao;
    GLuint ibo;
    int totalVerticles;
    std::vector<GLuint> indices;
    Material material;
    ObjectOrientation orientation;
    HitBox hitbox;

    Mesh();
    Mesh(const std::vector<Vertex>& vertices, const int& totalVerticles, Material material, const std::vector<GLuint>& indices);
    void rotate(const glm::vec3& angleDelta);
    void changeSize(const glm::vec3& scaleFactor);
    void movePos(const glm::vec3& delta);

    //void draw();
    void bindAlltoVao(std::vector<Vertex>vertices);
private:
};

#endif