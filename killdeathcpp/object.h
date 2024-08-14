#pragma once
#include <glm/glm.hpp>
#include "shader.h"
#include <GLFW/glfw3.h>

GLenum glCheckError_(const char* file, int line);
GLuint generateVBO(GLfloat points[], int size);
GLuint generateVAO();
GLuint generateIBO(int const Indices[], int size);


class Object {
public:
    GLuint vao;
    GLuint ibo;
    Shader shader;
    int totalVerticles;
    GLuint textureID;

    Object();
    Object(GLfloat verticles[], int verticlesByteSize, int totalVerticles, Shader shader, int vboCount, int textureID);
    Object(GLfloat verticles[], int verticlesByteSize, int totalVerticles, Shader shader, int vboCount, int textureID, int const indices[], int indicesSize);

    void changeSize(glm::vec3 vec);
    void movePos(glm::vec3 position);
    void make3DSquare();
    
    void setTexture(int textureID) {
        this->textureID = textureID;
    }
    void bindToVao(GLuint vbo, int vertexArray, int vecSize, int stride, int offset);
    void changeVbo(GLfloat* verticles, int verticlesByteSize) {
        GLfloat vbo = generateVBO(verticles, verticlesByteSize);
    } 
    void bindAlltoVao(GLfloat verticles[], int verticlesByteSize, int vboCount);

private:
};