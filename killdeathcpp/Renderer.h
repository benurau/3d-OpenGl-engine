#pragma once
#include "misc.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#define C_BPP         32
#define C_AREA_MEMORY_SIZE (C_RES_WIDTH * C_RES_HEIGHT * (C_BPP/8))
#define C_AREA_PIXELS 512*384
#define C_RES_MIDDLE (C_RES_WIDTH / 2)


float convertXtoFloat(double x);
float convertYtoFloat(double y);

typedef struct Mesh {
    const char* name;
    GLuint vao, fs, vs;
};

class Renderer {
public:
    std::unordered_map <const char*, Mesh> meshes;

    Renderer(GLFWwindow* window);
    GLuint generateVBO();
    GLuint generateVAO();
    void bindVao(GLuint vbo, GLuint vao, int vertexArrays, int vecSize);
    GLuint loadVertexShader(const char* filePath);
    GLuint loadFragmentShader(const char* filePath);
    void loadAllShaders();
    void drawTriange(Mesh mesh);

    void clear();
    void swapBuffers();
    GLuint create2DBitMapTexture(const char* filepath);
    void drawBackgroundTexture(GLuint textureID);
    void drawLine(float x1, float y1, float x2, float y2);
    void drawFixedLine(float x1, float y1, float x2, float y2, float length);
    void draw2DBitMap(GLuint textureID, float x1, float x2, float x3, float x4, float y1, float y2, float y3, float y4);

private:
    GLFWwindow* window;
    GLuint shaderProgram;
};
