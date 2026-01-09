#include "ObjectOrientation.h"
#include "misc.h"
#include "modelHelpers.h"

ObjectOrientation::ObjectOrientation()
    : position(0.0f), rotation(0.0f), scale(1.0f),
    modelMatrix(1.0f), view(1.0f), proj(1.0f) {
    setDefault();
}

void ObjectOrientation::rotate(const glm::vec3& angleDelta) {
    rotation += angleDelta;
    updateModelMatrix();
}

void ObjectOrientation::changeSize(const glm::vec3& scaleFactor) {
    scale += scaleFactor;
    updateModelMatrix();
}

void ObjectOrientation::movePos(const glm::vec3& delta) {
    position += delta;
    updateModelMatrix();
}

void ObjectOrientation::changeView(glm::vec3 position) {
    glm::mat4 viewMat = glm::mat4(1.0f);
    this->view = glm::translate(viewMat, position);
}

void ObjectOrientation::changeView(glm::mat4 view) {
    this->view = view;
}

void ObjectOrientation::changePerspective(float degrees) {
    this->proj = glm::perspective(glm::radians(degrees),
        static_cast<float>(C_RES_WIDTH) / C_RES_HEIGHT,
        0.1f, 100.0f);
}

void ObjectOrientation::setDefault() {
    position = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
    updateModelMatrix();
    changePerspective(45.0f);
}

void ObjectOrientation::updateModelMatrix() {
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, position);
    trans = glm::rotate(trans, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    trans = glm::rotate(trans, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    trans = glm::rotate(trans, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    trans = glm::scale(trans, scale);
    this->modelMatrix = trans;
}

void ObjectOrientation::debugPrint() {
    printf("modelMatrix:     ");
    PrintMat4(modelMatrix);
    printf("view:  ");
    PrintMat4(view);
    printf("proj:     ");
    PrintMat4(proj);
}


