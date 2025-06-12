#ifndef OBJECT_H
#define OBJECT_H


#include "shader.h"
#include "HitBox.h"


class Object {
public:
    GLuint vao;
    GLuint ibo;
    Shader shader;
    int totalVerticles;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    HitBox hitbox;


    Object();
    Object(const std::vector<Vertex>& vertices, const int& totalVerticles, const Shader& shader, const std::vector<Texture>& textures, const std::vector<GLuint>& indices);

    void draw();
    void movePos(const glm::vec3& delta);
    void changeSize(const glm::vec3& scaleFactor);
    void rotate(const glm::vec3& angleDelta);
    void changeView(glm::vec3 position);
    void changeView(glm::mat4 view);
    void changePerspective(float degrees);
    void updateModelMatrix();
    void setDefault();
    
    void setTexture(const std::vector<Texture>& textures) {
        this->textures = textures;
    }

    void bindAlltoVao(std::vector<Vertex>vertices);
    void Destroy();
private:
};

#endif