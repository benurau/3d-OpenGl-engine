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


GLuint generateVBO(GLfloat points[], int size) {
    assert(size > 0, "generateVBo with empty verticle array");
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, points, GL_DYNAMIC_DRAW);
    glCheckError();
    return vbo;
}

GLuint generateVAO() {
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glCheckError();
    return vao;
}

GLuint generateIBO(int const Indices[], int size) {
    assert(size > 0, "objectconstructor3 indices");
    GLuint IBO = 0;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, Indices, GL_DYNAMIC_DRAW);
    glCheckError();
    return IBO;
}

Object::Object(GLfloat verticles[], int verticlesByteSize, int totalVerticles, Shader shader, int vboCount, int textureID) {
    this->vao = generateVAO();
    assert(shader.ID > 0, "objectconstructor1 shader");
    assert(totalVerticles > 0, "objectconstructor1 totalVerticles");
    this->shader = shader;
    this->textureID = textureID;
    this->ibo = 0;
    this->totalVerticles = totalVerticles;
    bindAlltoVao(verticles, verticlesByteSize, vboCount);
}

Object::Object(GLfloat verticles[], int verticlesByteSize, int totalVerticles, Shader shader, int vboCount, int textureID, int const indices[], int indicesSize) {
    this->vao = generateVAO();
    assert(shader.ID > 0, "objectconstructor2 shader");
    assert(totalVerticles > 0, "objectconstructor2 totalVerticles");
    this->shader = shader;
    this->textureID = textureID;
    this->totalVerticles = totalVerticles;
    this->ibo = generateIBO(indices, indicesSize);
    bindAlltoVao(verticles, verticlesByteSize, vboCount);
}

void Object::bindToVao(GLuint vbo, int vertexArray, int vecSize, int stride, int offset) {
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(vertexArray, vecSize, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));
    glEnableVertexAttribArray(vertexArray);
    glCheckError();
}

void Object::bindAlltoVao(GLfloat verticles[], int verticlesByteSize, int vboCount) {
    GLfloat vbo = generateVBO(verticles, verticlesByteSize);
    assert(vboCount >= 0 && vboCount <= 4, vbocount);
    assert(verticlesByteSize > 0);
    //coordinates
    if (vboCount == 1) {
        bindToVao(vbo, 0, 3, 3, 0);
    }
    //colour
    else if(vboCount == 2) {
        bindToVao(vbo, 0, 3, 6, 0);
        bindToVao(vbo, 1, 3, 6, 3);
    }
    //texturePos
    else if (vboCount == 3 && textureID > 0) {
        bindToVao(vbo, 0, 3, 8, 0);
        bindToVao(vbo, 1, 3, 8, 3);
        bindToVao(vbo, 2, 2, 8, 6);
    }   
    else if (vboCount == 4 && textureID > 0) {
        bindToVao(vbo, 0, 3, 5, 0);
        bindToVao(vbo, 2, 2, 5, 3);
    }
    else {
        std::cout << "notextureid and 3vbocount";
        bindToVao(vbo, 0, 3, 6, 0);
        bindToVao(vbo, 1, 3, 6, 3);
    }
}

void Object::changeSize(glm::vec3 scale) {

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, scale);
    this->shader.setMat4("transform", trans);
}

void Object::movePos(glm::vec3 position) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    this->shader.setMat4("model", model);
}

void Object::make3DSquare() {
    glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)C_RES_WIDTH / (float)C_RES_HEIGHT, 0.1f, 100.0f);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)C_RES_WIDTH / (float)C_RES_HEIGHT, 0.1f, 100.0f);

    model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

    this->shader.setMat4("model", model);
    this->shader.setMat4("view", view);
    this->shader.setMat4("projection", proj);
}









