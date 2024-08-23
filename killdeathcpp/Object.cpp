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

Object::Object(GLfloat verticles[], int verticlesByteSize, int totalVerticles, Shader shader, VBOFormat format, int textureID) {
    this->vao = generateVAO();
    assert(shader.ID > 0, "objectconstructor1 shader");
    assert(totalVerticles > 0, "objectconstructor1 totalVerticles");
    this->shader = shader;
    this->textureID = textureID;
    this->ibo = 0;
    this->totalVerticles = totalVerticles;
    bindAlltoVao(verticles, verticlesByteSize, format);
}

Object::Object(GLfloat verticles[], int verticlesByteSize, int totalVerticles, Shader shader, VBOFormat format, int textureID, int const indices[], int indicesSize) {
    this->vao = generateVAO();
    assert(shader.ID > 0, "objectconstructor2 shader");
    assert(totalVerticles > 0, "objectconstructor2 totalVerticles");
    this->shader = shader;
    this->textureID = textureID;
    this->totalVerticles = totalVerticles;
    this->ibo = generateIBO(indices, indicesSize);
    bindAlltoVao(verticles, verticlesByteSize, format);
}

void Object::bindToVao(GLuint vbo, int vertexArray, int vecSize, int stride, int offset) {
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(vertexArray, vecSize, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));
    glEnableVertexAttribArray(vertexArray);
    glCheckError();
}

void Object::bindAlltoVao(GLfloat verticles[], int verticlesByteSize, VBOFormat format) {
    GLfloat vbo = generateVBO(verticles, verticlesByteSize);
    assert(verticlesByteSize > 0);
    if (format == P) {
        bindToVao(vbo, 0, 3, 3, 0);
    }
    if(format == PC) {
        bindToVao(vbo, 0, 3, 6, 0);
        bindToVao(vbo, 1, 3, 6, 3);
    }
    if (format == PCT && textureID > 0) {
        bindToVao(vbo, 0, 3, 8, 0);
        bindToVao(vbo, 1, 3, 8, 3);
        bindToVao(vbo, 2, 2, 8, 6);
    }   
    else if (format == PT && textureID > 0) {
        bindToVao(vbo, 0, 3, 5, 0);
        bindToVao(vbo, 2, 2, 5, 3);
    }
    else if (format == PT || format == PCT) {
        std::cout << "notextureid and T in VBOFormat";
        bindToVao(vbo, 0, 3, 6, 0);
        bindToVao(vbo, 1, 3, 6, 3);
    }
}

void Object::rotate(float degrees, glm::vec3 axises) {
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(degrees), axises);
    this->shader.setMat4("model", trans);
}

void Object::changeSize(glm::vec3 scale) {
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::scale(trans, scale);
    this->shader.setMat4("model", trans);
}

void Object::movePos(glm::vec3 position) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    this->shader.setMat4("model", model);
}

void Object::changeView(glm::vec3 position) {
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, position);
    this->shader.setMat4("view", view);
}

void Object::changeView(glm::mat4 view) {
    this->shader.setMat4("view", view);
}

void Object::changePerspective(float degrees) {
    glm::ortho(0.0f, (float)C_RES_WIDTH, 0.0f, (float)C_RES_HEIGHT, 0.1f, 100.0f);
    glm::mat4 proj = glm::perspective(glm::radians(degrees), (float)C_RES_WIDTH / (float)C_RES_HEIGHT, 0.1f, 100.0f);
    this->shader.setMat4("projection", proj);
}

void Object::make3DSquare() {
    rotate(-55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    changePerspective(45.0f);
    rotate((float)glfwGetTime() * 50.0f, glm::vec3(0.5f, 1.0f, 0.0f));
}









