#ifndef MESH_H
#define MESH_H

#include "Material.h"


class Mesh {
public:
    GLuint vao;
    GLuint ibo;
    GLfloat vbo;
    std::vector<GLuint> indices;
    std::vector<Vertex> vertices;
    Material material;
    int materialIndex;


    Mesh();
    Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
    void uploadToGPU();
    void bindAlltoVao();
private:
};

#endif