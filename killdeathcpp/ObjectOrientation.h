#ifndef OBJECT_ORIENTATION_H
#define OBJECT_ORIENTATION_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "shader.h"
#include "HitBox.h"


class ObjectOrientation {
public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::mat4 modelMatrix;
    glm::mat4 view;
    glm::mat4 proj;

    ObjectOrientation();
    void rotate(const glm::vec3& angleDelta);
    void changeSize(const glm::vec3& scaleFactor);
    void movePos(const glm::vec3& delta);
    void changeView(glm::vec3 position);
    void changeView(glm::mat4 view);
    void changePerspective(float degrees);
    void updateShader(Shader* shader) const;
    void setDefault();

private:
    void updateModelMatrix();   
};

#endif
