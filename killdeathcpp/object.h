#pragma once
#include <glm/glm.hpp>
#include "shader.h"
#include <GLFW/glfw3.h>

enum VBOFormat {
    P,
    PC,
    PCT,
    PT
};


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
    Object(GLfloat verticles[], int verticlesByteSize, int totalVerticles, Shader shader, VBOFormat format, int textureID);
    Object(GLfloat verticles[], int verticlesByteSize, int totalVerticles, Shader shader, VBOFormat format, int textureID, int const indices[], int indicesSize);

    void rotate(float degrees, glm::vec3 axises);
    void changeSize(glm::vec3 vec);
    void movePos(glm::vec3 position);
    void changeView(glm::vec3 position);
    void changeView(glm::mat4 view);
    void changePerspective(float degrees);
    void make3DSquare();
    
    void setTexture(int textureID) {
        this->textureID = textureID;
    }
    void bindToVao(GLuint vbo, int vertexArray, int vecSize, int stride, int offset);
    void changeVbo(GLfloat* verticles, int verticlesByteSize) {
        GLfloat vbo = generateVBO(verticles, verticlesByteSize);
    } 
    void bindAlltoVao(GLfloat verticles[], int verticlesByteSize, VBOFormat format);

private:
};