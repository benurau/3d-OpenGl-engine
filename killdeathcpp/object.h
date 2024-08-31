#pragma once
#include "shader.h"
#include <GLFW/glfw3.h>
#include "HitBox.h"


GLenum glCheckError_(const char* file, int line);
GLuint generateVBO(std::vector<Vertex> vertices);
GLuint generateVAO();
GLuint generateIBO(std::vector<GLuint> indices);

class Object {
public:
    GLuint vao;
    GLuint ibo;
    Shader shader;
    int totalVerticles;
    GLuint textureID;
    HitBox hitbox;

    Object();
    Object(std::vector<Vertex>vertices, int totalVerticles, Shader shader, int textureID);
    Object(std::vector<Vertex>vertices, int totalVerticles, Shader shader, int textureID, std::vector<GLuint> indices);

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
    /*void changeVbo(GLfloat* verticles, int verticlesByteSize) {
        GLfloat vbo = generateVBO(verticles, verticlesByteSize);
    } */
    void bindAlltoVao(std::vector<Vertex>vertices);

private:
};