#define GLM_ENABLE_EXPERIMENTAL

#include "Mesh.h"
#include "openglHelpers.h"
#include <glm/gtx/string_cast.hpp>


Mesh::Mesh() {};

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














