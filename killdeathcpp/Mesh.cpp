#define GLM_ENABLE_EXPERIMENTAL

#include "Mesh.h"
#include "openglHelpers.h"
#include <glm/gtx/string_cast.hpp>



Mesh::Mesh() : vao(0), ibo(0), vbo(0), materialIndex(-1) {};

Mesh::~Mesh() {
    if (ownsGpuResources) {
        if (vao) glDeleteVertexArrays(1, &vao);
        if (vbo) glDeleteBuffers(1, &vbo);
        if (ibo) glDeleteBuffers(1, &ibo);
    }
}

Mesh::Mesh(const Mesh& other)
    : vao(other.vao), ibo(other.ibo), vbo(other.vbo),
      indices(other.indices), vertices(other.vertices),
      material(other.material), materialIndex(other.materialIndex),
      ownsGpuResources(false) {}

Mesh& Mesh::operator=(const Mesh& other) {
    if (this != &other) {
        if (ownsGpuResources) {
            if (vao) glDeleteVertexArrays(1, &vao);
            if (vbo) glDeleteBuffers(1, &vbo);
            if (ibo) glDeleteBuffers(1, &ibo);
        }
        vao = other.vao; ibo = other.ibo; vbo = other.vbo;
        indices = other.indices;
        vertices = other.vertices;
        material = other.material;
        materialIndex = other.materialIndex;
        ownsGpuResources = false;
    }
    return *this;
}

Mesh::Mesh(Mesh&& other) noexcept
    : vao(other.vao), ibo(other.ibo), vbo(other.vbo),
      indices(std::move(other.indices)), vertices(std::move(other.vertices)),
      material(std::move(other.material)), materialIndex(other.materialIndex),
      ownsGpuResources(other.ownsGpuResources) {
    other.vao = 0;
    other.ibo = 0;
    other.vbo = 0;
    other.ownsGpuResources = false;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        if (ownsGpuResources) {
            if (vao) glDeleteVertexArrays(1, &vao);
            if (vbo) glDeleteBuffers(1, &vbo);
            if (ibo) glDeleteBuffers(1, &ibo);
        }
        vao = other.vao; ibo = other.ibo; vbo = other.vbo;
        indices = std::move(other.indices);
        vertices = std::move(other.vertices);
        material = std::move(other.material);
        materialIndex = other.materialIndex;
        ownsGpuResources = other.ownsGpuResources;
        other.vao = 0; other.ibo = 0; other.vbo = 0;
        other.ownsGpuResources = false;
    }
    return *this;
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices) {
    this->indices = indices;
    this->vertices = vertices;
    uploadToGPU();
}

void Mesh::uploadToGPU()
{
    vao = generateVAO();
    vbo = generateVBO(vertices);
    ibo = indices.empty() ? 0 : generateIBO(indices);
    bindAlltoVao();
}


void Mesh::bindAlltoVao() {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    bindToVao(0, 3, sizeof(Vertex), offsetof(Vertex, position));
    bindToVao(1, 3, sizeof(Vertex), offsetof(Vertex, normal));
    bindToVao(2, 2, sizeof(Vertex), offsetof(Vertex, texCoords)); 
    bindToVao(3, 3, sizeof(Vertex), offsetof(Vertex, Tangent));
    bindToVao(4, 3, sizeof(Vertex), offsetof(Vertex, Bitangent));
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_UNSIGNED_INT,  sizeof(Vertex), (void*)offsetof(Vertex, joints));
    bindToVao(6, 4, sizeof(Vertex), offsetof(Vertex, weights));

    if (ibo > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    }
    glBindVertexArray(0);
}














