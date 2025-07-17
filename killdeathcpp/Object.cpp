#define GLM_ENABLE_EXPERIMENTAL

#include "Object.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "openglHelpers.h"
#include <glm/gtx/string_cast.hpp>


Object::Object(const std::vector<Vertex>& vertices, const int& totalVerticles, const Shader& shader, const std::vector<Texture>& textures, const std::vector<GLuint>& indices) {
    this->vao = generateVAO();
    assert(shader.ID > 0, "objectconstructor shader");
    assert(totalVerticles > 0, "objectconstructor totalVerticles");
    this->shader = shader;
    this->textures = textures;
    this->totalVerticles = totalVerticles;
    this->indices = indices;
    this->hitbox = HitBox(vertices, indices);
    this->ibo = generateIBO(indices);
    bindAlltoVao(vertices);
    setDefault();
}


void Object::bindAlltoVao(std::vector<Vertex>vertices) {
    GLfloat vbo = generateVBO(vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    int loc = 0;
    loc = glGetAttribLocation(shader.ID, "aPos");
    if (loc != -1) {
        bindToVao(vao, loc, 0, 3, sizeof(Vertex), 0);
    }
    loc = glGetAttribLocation(shader.ID, "aNormal");
    if (loc != -1) {
        bindToVao(vao, loc, 1, 3, sizeof(Vertex), offsetof(Vertex, normal));
    }
    loc = glGetAttribLocation(shader.ID, "aTexCoords");
    if (loc != -1) {
        bindToVao(vao, loc, 2, 2, sizeof(Vertex), offsetof(Vertex, texCoords));
    }
    if (ibo > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    }
    glBindVertexArray(0);
}

void Object::draw() {
    std::cout << "[Draw] Using shader ID: " << shader.ID << std::endl;
    shader.use();
    checkGLError("usingshader");
    updateShader(&shader);
    checkGLError("setObjectShaderVlaues(model)");

    if (!textures.empty()) {
        shader.setBool("useTexture", true);
        std::cout << "[Draw] Binding " << textures.size() << " textures." << std::endl;
        int diffuseNr = 1;
        int specularNr = 1;

        for (int i = 0; i < textures.size(); ++i) {
            
            glActiveTexture(GL_TEXTURE0 + i);
            checkGLError("glActiveTexture");

            std::string name;
            if (textures[i].type == "texture_diffuse") {
                name = "texture_diffuse" + std::to_string(diffuseNr++);
            }
            else if (textures[i].type == "texture_specular") {
                name = "texture_specular" + std::to_string(specularNr++);
            }
            else {
                std::cout << "[Draw] Unknown texture type: " << textures[i].type << std::endl;
            }

            GLint loc = glGetUniformLocation(shader.ID, name.c_str());
            GLint matLoc = glGetUniformLocation(shader.ID, ("material." + name).c_str());
            if (loc != -1) {
                glUniform1i(loc, i);
                checkGLError("glUniform1i " + name);
            }
            else if (matLoc != -1)
            {
                glUniform1i(matLoc, i);
                checkGLError("glUniform1i material." + name);
            }
            else {
                std::cout << "[Warn] Uniform not found or inactive: " << name << std::endl;
            }
            
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
            checkGLError("glBindTexture " + name);
        }
    }
    else {
        shader.setBool("useTexture", false);
        std::cout << "[Draw] No textures to bind." << std::endl;
    }

    std::cout << "[Draw] Binding VAO ID: " << vao << std::endl;
    glBindVertexArray(vao);
    checkGLError("glBindVertexArray");

    if (!indices.empty()) {
        std::cout << "[Draw] Drawing with glDrawElements, count: " << indices.size() << std::endl;
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        checkGLError("glDrawElements");
    }
    else {
        std::cout << "[Draw] Drawing with glDrawArrays, count: " << totalVerticles << std::endl;
        glDrawArrays(GL_TRIANGLES, 0, totalVerticles);
        checkGLError("glDrawArrays");
    }

    glBindVertexArray(0);
    std::cout << "[Draw] Unbound VAO." << std::endl;
    checkGLError("unboundvao");

    for (int i = 0; i < static_cast<int>(textures.size()); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
        std::cout << "[Draw] Unbound texture unit " << i << std::endl;
        checkGLError("unactivatetexture");
    }

    glActiveTexture(GL_TEXTURE0);
    checkGLError("activatetexture0");
    std::cout << "[Draw] Reset active texture to GL_TEXTURE0." << std::endl;
}






void Object::rotate(const glm::vec3& angleDelta) {
    rotation += angleDelta;
    updateModelMatrix();
}

void Object::changeSize(const glm::vec3& scaleFactor) {
    scale += scaleFactor;
    updateModelMatrix();
}

void Object::movePos(const glm::vec3& delta) {
    std::cout << delta.y;
    position += delta;
    updateModelMatrix();
}

void Object::changeView(glm::vec3 position) {
    glm::mat4 view = glm::mat4(1.0f);
    this->view = glm::translate(view, position);
    hitbox.setViewMatrix(this->view);
}

void Object::changeView(glm::mat4 view) {
    this->view = view;
    hitbox.setViewMatrix(this->view);
}

void Object::changePerspective(float degrees) {
    this->proj = glm::perspective(glm::radians(degrees), (float)C_RES_WIDTH / (float)C_RES_HEIGHT, 0.1f, 100.0f);
    hitbox.setProjectionMatrix(this->proj);
}

void Object::setDefault() {
    shader.use();
    position = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
    updateModelMatrix();
    changePerspective(45.0f);
    
}

void Object::updateModelMatrix() {
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, position);
    trans = glm::rotate(trans, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    trans = glm::rotate(trans, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    trans = glm::rotate(trans, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    trans = glm::scale(trans, scale);
    this->modelMatrix = trans;
    hitbox.setModelMatrix(this->modelMatrix);
}

void Object::updateShader(Shader* shader) const {
    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", proj);
    shader->setMat4("model", modelMatrix);
}


void Object::Destroy() {
    this->hitbox.Destroy();
}









