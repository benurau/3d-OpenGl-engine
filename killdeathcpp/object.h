#ifndef OBJECT_H
#define OBJECT_H


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
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    HitBox hitbox;


    Object();
    /*Object(std::vector<Vertex>vertices, int totalVerticles, Shader shader, int textureID);*/
    Object(std::vector<Vertex>vertices, int totalVerticles, Shader shader, int textureID, std::vector<GLuint> indices);

    void movePos(const glm::vec3& delta);
    void changeSize(const glm::vec3& scaleFactor);
    void rotate(const glm::vec3& angleDelta);
    void changeView(glm::vec3 position);
    void changeView(glm::mat4 view);
    void changePerspective(float degrees);
    void updateModelMatrix();
    void setDefault();
    
    void setTexture(int textureID) {
        this->textureID = textureID;
    }
    void bindToVao(GLuint vbo, int vertexArray, int vecSize, int stride, int offset);
    /*void changeVbo(GLfloat* verticles, int verticlesByteSize) {
        GLfloat vbo = generateVBO(verticles, verticlesByteSize);
    } */
    void bindAlltoVao(std::vector<Vertex>vertices);
    void Destroy();
private:
};

#endif