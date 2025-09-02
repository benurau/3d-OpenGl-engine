#define GLM_ENABLE_EXPERIMENTAL

#include "Mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "openglHelpers.h"
#include <glm/gtx/string_cast.hpp>


Mesh::Mesh(const std::vector<Vertex>& vertices, const int& totalVerticles, Material material, const std::vector<GLuint>& indices) {
    this->vao = generateVAO();
    this->material = material;
    this->totalVerticles = totalVerticles;
    this->indices = indices;
    this->hitbox = HitBox(vertices, indices);
    this->ibo = generateIBO(indices);
    this->orientation = ObjectOrientation();
    bindAlltoVao(vertices);
}

void Mesh::bindAlltoVao(std::vector<Vertex>vertices) {
    GLfloat vbo = generateVBO(vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    bindToVao(vao, 0, 3, sizeof(Vertex), 0);
    bindToVao(vao, 1, 3, sizeof(Vertex), offsetof(Vertex, normal));
    bindToVao(vao, 2, 2, sizeof(Vertex), offsetof(Vertex, texCoords)); 
    if (ibo > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    }
    glBindVertexArray(0);
}

void Mesh::rotate(const glm::vec3& angleDelta) {
    orientation.rotate(angleDelta);
    hitbox.updateModelMatrix(orientation.modelMatrix);
}
void Mesh::changeSize(const glm::vec3& scaleFactor) {
    orientation.changeSize(scaleFactor);
    hitbox.updateModelMatrix(orientation.modelMatrix);
}
void Mesh::movePos(const glm::vec3& delta) {
    orientation.movePos(delta);
    hitbox.updateModelMatrix(orientation.modelMatrix);
}

//void Object::draw() {
//    //std::cout << "[Draw] Using shader ID: " << shader.ID << std::endl;
//    checkGLError("usingshader");
//    updateShader(&shader);
//    checkGLError("setObjectShaderVlaues(model)");
//
//    if (!textures.empty()) {
//        shader.setBool("useTexture", true);
//        //std::cout << "[Draw] Binding " << textures.size() << " textures." << std::endl;
//        int diffuseNr = 1;
//        int specularNr = 1;
//
//        for (int i = 0; i < textures.size(); ++i) {
//            
//            glActiveTexture(GL_TEXTURE0 + i);
//            checkGLError("glActiveTexture");
//
//            std::string name;
//            if (textures[i].type == "texture_diffuse") {
//                name = "texture_diffuse" + std::to_string(diffuseNr++);
//            }
//            else if (textures[i].type == "texture_specular") {
//                name = "texture_specular" + std::to_string(specularNr++);
//            }
//            else {
//                //std::cout << "[Draw] Unknown texture type: " << textures[i].type << std::endl;
//            }
//
//            GLint loc = glGetUniformLocation(shader.ID, name.c_str());
//            GLint matLoc = glGetUniformLocation(shader.ID, ("material." + name).c_str());
//            if (loc != -1) {
//                glUniform1i(loc, i);
//                checkGLError("glUniform1i " + name);
//            }
//            else if (matLoc != -1)
//            {
//                glUniform1i(matLoc, i);
//                checkGLError("glUniform1i material." + name);
//            }
//            else {
//                //std::cout << "[Warn] Uniform not found or inactive: " << name << std::endl;
//            }
//            
//            glBindTexture(GL_TEXTURE_2D, textures[i].id);
//            checkGLError("glBindTexture " + name);
//        }
//    }
//    else {
//        shader.setBool("useTexture", false);
//        //std::cout << "[Draw] No textures to bind." << std::endl;
//    }
//
//    //std::cout << "[Draw] Binding VAO ID: " << vao << std::endl;
//    glBindVertexArray(vao);
//    checkGLError("glBindVertexArray");
//
//    if (!indices.empty()) {
//        //std::cout << "[Draw] Drawing with glDrawElements, count: " << indices.size() << std::endl;
//        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
//        checkGLError("glDrawElements");
//    }
//    else {
//        //std::cout << "[Draw] Drawing with glDrawArrays, count: " << totalVerticles << std::endl;
//        glDrawArrays(GL_TRIANGLES, 0, totalVerticles);
//        checkGLError("glDrawArrays");
//    }
//
//    glBindVertexArray(0);
//    //std::cout << "[Draw] Unbound VAO." << std::endl;
//    checkGLError("unboundvao");
//
//    for (int i = 0; i < static_cast<int>(textures.size()); ++i) {
//        glActiveTexture(GL_TEXTURE0 + i);
//        glBindTexture(GL_TEXTURE_2D, 0);
//        //std::cout << "[Draw] Unbound texture unit " << i << std::endl;
//        checkGLError("unactivatetexture");
//    }
//
//    glActiveTexture(GL_TEXTURE0);
//    checkGLError("activatetexture0");
//    //std::cout << "[Draw] Reset active texture to GL_TEXTURE0." << std::endl;
//}












