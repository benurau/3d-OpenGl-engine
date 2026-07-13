#ifndef MESH_H
#define MESH_H

#include "Material.h"


class Mesh {
public:
    GLuint vao;
    GLuint ibo;
    GLuint vbo;
    std::vector<GLuint> indices;
    std::vector<Vertex> vertices;
    Material material;
    int materialIndex;

    Mesh();
    Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
    ~Mesh();
    Mesh(const Mesh& other);
    Mesh& operator=(const Mesh& other);
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;
    void uploadToGPU();
    void bindAlltoVao();
private:
    bool ownsGpuResources = true;
};

#endif