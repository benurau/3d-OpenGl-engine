#pragma once
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


GLenum glCheckError_(const char* file, int line);

struct Mesh {
    const char* name;
    GLuint vao;
};

struct Texture {
    const char* type;
    int id;
};

GLuint generateVBO(float points[], int size);
GLuint generateVAO();
GLuint generateIBO(int const Indices[], int size);
void bindToVao(GLuint vbo, GLuint vao, int vertexArray, int vecSize, int stride, int offset);

class Renderer {
public:
    std::unordered_map <const char*, Shader> shaders;
    std::unordered_map <const char*, GLuint> vaos;
    Renderer(GLFWwindow* window);
    void loadAllShaders();

    void drawLine(GLuint vao, Shader shader, GLfloat points[], int size);
    void drawTriange(GLuint vao, Shader shader, GLfloat points[], int size);
    void drawRectangle(GLuint vao, Shader shader, GLfloat points[], int size);
    void draw2DBitMap(GLuint vao, Shader shader, GLfloat points[], int size, GLuint textureID);

    void draw3DSquare(Shader shader);
    void drawCube(GLuint vao, Shader shader, GLfloat points[], int size, GLuint textureID);

    void changeSize(Shader shader, glm::vec3 vec);
    void movePos(Shader shader, glm::vec3 position);
    void make3DSquare(Shader shader);

    void clear();
    void swapBuffers();
    GLuint create2DBitMapTexture(const char* filepath);
    void drawFixedLine(GLuint vao, Shader shader, GLfloat points[], int size, float length);
    

private:
    GLFWwindow* window;
};
