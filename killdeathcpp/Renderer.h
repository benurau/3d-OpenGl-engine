#ifndef RENDERER_H
#define RENDERER_H


#include "misc.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <unordered_map>
#include <glm/glm.hpp>


#define C_BPP         32
#define C_AREA_MEMORY_SIZE (C_RES_WIDTH * C_RES_HEIGHT * (C_BPP/8))
#define C_AREA_PIXELS 512*384
#define C_RES_MIDDLE (C_RES_WIDTH / 2)

struct Light {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};


class Renderer {
public:
    Renderer(GLFWwindow* window);

    void clear();
    void swapBuffers();

    

private:
    GLFWwindow* window;
};
#endif