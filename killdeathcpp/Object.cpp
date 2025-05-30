#include "Object.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "misc.h"

GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 


GLuint generateVBO(std::vector<Vertex>vertices) {
    //assert(size > 0, "generateVBo with empty verticle array");
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    glCheckError();
    return vbo;
}

GLuint generateVAO() {
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glCheckError();
    return vao;
}

GLuint generateIBO(std::vector<GLuint> indices) {
    assert(indices.size() > 0, "objectconstructor3 indices");
    GLuint IBO = 0;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_DYNAMIC_DRAW);
    glCheckError();
    return IBO;
}

//Object::Object(std::vector<Vertex>vertices, int totalVerticles, Shader shader, int textureID) {
//    this->vao = generateVAO();
//    assert(shader.ID > 0, "objectconstructor1 shader");
//    assert(totalVerticles > 0, "objectconstructor1 totalVerticles");
//    this->shader = shader;
//    this->textureID = textureID;
//    this->ibo = 0;
//    this->totalVerticles = totalVerticles;
//    bindAlltoVao(vertices);
//}

Object::Object(std::vector<Vertex>vertices, int totalVerticles, Shader shader, int textureID, std::vector<GLuint> indices) {
    this->vao = generateVAO();
    assert(shader.ID > 0, "objectconstructor2 shader");
    assert(totalVerticles > 0, "objectconstructor2 totalVerticles");
    this->shader = shader;
    this->textureID = textureID;
    this->totalVerticles = totalVerticles;
    this->hitbox = HitBox(vertices, indices);
    this->ibo = generateIBO(indices);
    bindAlltoVao(vertices);
    setDefault();
}

void Object::bindToVao(GLuint vbo, int vertexArray, int vecSize, int stride, int offset) {
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(vertexArray, vecSize, GL_FLOAT, GL_FALSE, stride, (void*)offset);
    glEnableVertexAttribArray(vertexArray);
    glCheckError();
}

void Object::bindAlltoVao(std::vector<Vertex>vertices) {
    GLfloat vbo = generateVBO(vertices);
    bindToVao(vbo, 0, 3, sizeof(Vertex), 0);
    bindToVao(vbo, 1, 3, sizeof(Vertex), offsetof(Vertex, normal));
    bindToVao(vbo, 2, 2, sizeof(Vertex), offsetof(Vertex, texCoords));
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
    view = glm::translate(view, position);
    this->shader.setMat4("view", view);
    hitbox.setViewMatrix(view);
}

void Object::changeView(glm::mat4 view) {
    this->shader.setMat4("view", view);
    hitbox.setViewMatrix(view);
}

void Object::changePerspective(float degrees) {
    glm::ortho(0.0f, (float)C_RES_WIDTH, 0.0f, (float)C_RES_HEIGHT, 0.1f, 100.0f);
    glm::mat4 proj = glm::perspective(glm::radians(degrees), (float)C_RES_WIDTH / (float)C_RES_HEIGHT, 0.1f, 100.0f);
    this->shader.setMat4("projection", proj);
    hitbox.setProjectionMatrix(proj);
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
    modelMatrix = trans;
    shader.setMat4("model", modelMatrix);
    hitbox.setModelMatrix(modelMatrix);
}


void Object::Destroy() {
    this->hitbox.Destroy();
}









