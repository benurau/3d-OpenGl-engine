#include "renderer.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "misc.h"

GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

Renderer::Renderer(GLFWwindow* window) : window(window){}
void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::swapBuffers() {
    glfwSwapBuffers(window);
}

void Renderer::drawFixedLine(GLuint vao, Shader shader, GLfloat points[], int size, float length) {
    float dirX = points[3] - points[0];
    float dirY = points[4] - points[1];;
    float distance = sqrt(dirX * dirX + dirY * dirY);

    if (distance == 0) {
        return;
    }
    float normDirX = dirX / distance;
    float normDirY = dirY / distance;
    float finalX = points[0] + normDirX * length;
    float finalY = points[1] + normDirY * length;

    GLfloat newPoints[] =
    {
        points[0], points[1], points[2],
        finalX, finalY, points[5]
    };

    drawLine(vao , shader , newPoints, size);
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

    glGenTextures(1, &textureID);
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

void Renderer::loadAllShaders() {
    shaders["triangle"] = Shader("..\\shaders\\triangle_vs.txt", "..\\shaders\\rainbow_fs.txt");
    vaos["triangle"] = generateVAO();
    shaders["guad"] = Shader("..\\shaders\\guad_vs.txt", "..\\shaders\\rainbow_fs.txt");
    vaos["guad"] = generateVAO();
    shaders["line"] = Shader("..\\shaders\\line_vs.txt", "..\\shaders\\line_fs.txt");
    vaos["line"] = generateVAO();
    shaders["guadtexture"] = Shader("..\\shaders\\background_vs.txt", "..\\shaders\\background_fs.txt");
    vaos["guadtexture"] = generateVAO();
    shaders["guad3d"] = Shader("..\\shaders\\guad3d_vs.txt", "..\\shaders\\guad3d_fs.txt");
}

GLuint generateVBO(GLfloat points[], int size) {
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, points, GL_DYNAMIC_DRAW);
    glCheckError();
    return vbo;
}

GLuint generateVAO() {
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glCheckError();
    return vao;
}

GLuint generateIBO(int const Indices[], int size) {
    GLuint IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, Indices, GL_DYNAMIC_DRAW);
    glCheckError();
    return IBO;
}

void bindToVao(GLuint vbo, GLuint vao, int vertexArray, int vecSize, int stride, int offset) {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(vertexArray, vecSize, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));
    glEnableVertexAttribArray(vertexArray);
    glCheckError();
}


void Renderer::drawLine(GLuint vao, Shader shader, GLfloat points[], int size) {
    assert(vao>0);
    assert(shader.ID != 0);
    GLuint vbo = generateVBO(points, size);
    bindToVao(vbo, vao, 0, 3, 3, 0);
    //bindToVao(vbo, vao, 0, 3, 6, 0);
    shader.use();
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, 2);
}

void Renderer::drawTriange(GLuint vao, Shader shader, GLfloat points[], int size) {
    assert(vao > 0);
    assert(shader.ID != 0);
    int indexBufferData[] =
    {
        0, 1, 2,
    };
    GLuint vbo = generateVBO(points, size);
    bindToVao(vbo, vao, 0, 3, 3, 0);
    //bindToVao(vbo, vao, 1, 3, 6, 3);
    shader.use();
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, generateIBO(indexBufferData, sizeof(indexBufferData)));
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0); 
}


void Renderer::drawRectangle(GLuint vao, Shader shader, GLfloat points[], int size) {
    assert(vao > 0);
    assert(shader.ID != 0);
    int indexBufferData[] =
    {
        0, 1, 2,
        0, 2, 3
    };
    GLuint vbo = generateVBO(points, size);
    bindToVao(vbo, vao, 0, 3, 6, 0);
    bindToVao(vbo, vao, 1, 3, 6, 3);
    shader.use();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, generateIBO(indexBufferData, sizeof(indexBufferData)));
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


void Renderer::draw2DBitMap(GLuint vao, Shader shader, GLfloat points[], int size, GLuint textureID){
    assert(vao > 0);
    assert(shader.ID != 0);
    int indexBufferData[] =
    {
        0, 1, 3,
        1, 2, 3
    };

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLuint vbo = generateVBO(points, size);
    bindToVao(vbo, vao, 0, 3, 8, 0);
    bindToVao(vbo, vao, 1, 3, 8, 3);
    bindToVao(vbo, vao, 2, 2, 8, 6);
    shader.use();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, generateIBO(indexBufferData, sizeof(indexBufferData)));
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::drawCube(GLuint vao, Shader shader, GLfloat points[], int size, GLuint textureID) {
    assert(vao > 0);
    assert(shader.ID != 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLuint vbo = generateVBO(points, size);
    bindToVao(vbo, vao, 0, 3, 5, 0);
    bindToVao(vbo, vao, 2, 2, 5, 3);
    shader.use();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES,0, 36);
}

void Renderer::changeSize(Shader shader, glm::vec3 scale) {

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, scale);
    shader.setMat4("transform", trans);
}

void Renderer::movePos(Shader shader, glm::vec3 position) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    shader.setMat4("model", model);
}

void Renderer::make3DSquare(Shader shader) {
    glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)C_RES_WIDTH / (float)C_RES_HEIGHT, 0.1f, 100.0f);
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)C_RES_WIDTH / (float)C_RES_HEIGHT, 0.1f, 100.0f);

    model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", proj);

    glCheckError();
}



