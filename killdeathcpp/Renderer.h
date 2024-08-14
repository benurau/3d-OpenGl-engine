#pragma once
#include "misc.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <unordered_map>
#include <glm/glm.hpp>
#include "Object.h"

#define C_BPP         32
#define C_AREA_MEMORY_SIZE (C_RES_WIDTH * C_RES_HEIGHT * (C_BPP/8))
#define C_AREA_PIXELS 512*384
#define C_RES_MIDDLE (C_RES_WIDTH / 2)


struct Mesh {
    const char* name;
    GLuint vao;
};

struct Texture {
    const char* type;
    int id;
};

class Renderer {
public:
    std::unordered_map <const char*, Shader> shaders;
    std::unordered_map <const char*, GLuint> vaos;
    Renderer(GLFWwindow* window);
    void loadAllShaders();

    void drawLine(Object object);
    void drawObject(Object object);

    void clear();
    void swapBuffers();
    GLuint create2DBitMapTexture(const char* filepath);
    void drawFixedLine(GLuint vao, Shader shader, GLfloat points[], int size, float length);
    

private:
    GLFWwindow* window;
};
