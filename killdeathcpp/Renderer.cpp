#include "renderer.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <glm/vec3.hpp>
#include <fstream>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "misc.h"


float convertXtoFloat(double x) {
    float result = (2.0f * x) / C_RES_WIDTH - 1.0f;
    return result;
}

float convertYtoFloat(double y) {
    float result = 1.0f - (2.0f * y) / C_RES_HEIGHT;
    return result;
}

void _print_shader_info_log(GLuint shader_index) {
    int max_length = 2048;
    int actual_length = 0;
    char shader_log[2048];
    glGetShaderInfoLog(shader_index, max_length, &actual_length, shader_log);
    printf("shader info log for GL index %u:\n%s\n", shader_index, shader_log);
}

const char* readShaderFile(const std::string& filePath) {
    std::ifstream t(filePath);
    if (!t.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return nullptr;
    }
    std::stringstream buffer;
    buffer << t.rdbuf();
    t.close();
    return buffer.str().c_str();
}



Renderer::Renderer(GLFWwindow* window) : window(window){}
void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::swapBuffers() {
    glfwSwapBuffers(window);
}

void Renderer::drawLine(float x1, float y1, float x2, float y2) {
    if (x1 < -1 || x1 > 1) {
        x1 = convertXtoFloat(x1);
    }
    if (y1 < -1 || y1 > 1) {
        y1 = convertYtoFloat(y1);
    } 
    if (x2 < -1 || x2 > 1) {
        x2 = convertXtoFloat(x2);
    }
    if (y2 < -1 || y2 > 1) {
        y2 = convertYtoFloat(y2);
    }

    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}


void Renderer::drawFixedLine(float x1, float y1, float x2, float y2, float length) {
    float dirX = x2 - x1;
    float dirY = y2 - y1;
    float distance = sqrt(dirX * dirX + dirY * dirY);

    if (distance == 0) {
        return;
    }
    float normDirX = dirX / distance;
    float normDirY = dirY / distance;
    float finalX = x1 + normDirX * length;
    float finalY = y1 + normDirY * length;

    drawLine(x1, y1, finalX, finalY);
}

GLuint Renderer::create2DBitMapTexture(const char* filepath) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
    if (data == NULL) {
        printf("Error loading image: %s\n", stbi_failure_reason());
        return 1;
    }

    GLenum format = 0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

    GLuint textureID = 0;
    if (textureID == 0) {
        glGenTextures(1, &textureID);
    }
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return textureID;
}

GLuint Renderer::generateVBO() {
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    return vbo;
}

GLuint Renderer::generateVAO() {
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    return vao;
}

void Renderer::bindVao(GLuint vbo, GLuint vao, int vertexArrays, int vecSize) {
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(vertexArrays, vecSize, GL_FLOAT, GL_FALSE, 0, NULL);
}


GLuint Renderer::loadVertexShader(const char* filePath) {
    const char* vertex_shader = readShaderFile(filePath);
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    int params = -1;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &params);
    if (GL_TRUE != params) {
        fprintf(stderr, "ERROR: GL shader index %i did not compile\n", vs);
        _print_shader_info_log(vs);
        return false; 
    }
    return vs;
}

GLuint Renderer::loadFragmentShader(const char* filePath) {
    const char* fragment_shader = readShaderFile(filePath);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);
    int params = -1;
    glGetShaderiv(fs, GL_COMPILE_STATUS, &params);
    if (GL_TRUE != params) {
        fprintf(stderr, "ERROR: GL shader index %i did not compile\n", fs);
        _print_shader_info_log(fs);
        return false;
    }
    return fs;
}

void Renderer::loadAllShaders() {
    Mesh mesh = {0};
    mesh.vs = loadVertexShader("..\\assets\\triangle_vs.txt");
    mesh.fs = loadFragmentShader("..\\assets\\rainbow_fs.txt");
    mesh.vao = generateVAO();
    mesh.name = "triangle";
    meshes[mesh.name] = mesh;
}

void Renderer::drawTriange(Mesh mesh) {
    float points[] = {
       0.0f,  0.5f,  0.0f,
       0.5f, -0.5f,  0.0f,
      -0.5f, -0.5f,  0.0f
    };
    float colours[] = {
      1.0f, 0.0f,  0.0f,
      0.0f, 1.0f,  0.0f,
      0.0f, 0.0f,  1.0f
    };
    GLuint vbo = generateVBO();
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);
    GLuint colours_vbo = generateVBO();
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), colours, GL_STATIC_DRAW);
    bindVao(vbo ,mesh.vao, 0, 3);
    bindVao(colours_vbo, mesh.vao, 1, 3);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, mesh.fs);
    glAttachShader(shaderProgram, mesh.vs);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    glBindVertexArray(mesh.vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    

}

void Renderer::drawBackgroundTexture(GLuint textureID) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();
    glDisable(GL_BLEND);
}

void Renderer::draw2DBitMap(GLuint textureID, float x1, float x2, float x3, float x4, float y1, float y2, float y3, float y4) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x1, y1);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x2, y2);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x3, y3);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x4, y4);
    glEnd();
    glDisable(GL_BLEND);
}



