#include "renderer.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "misc.h"

float convertXtoFloat(double x) {
    // Perform the conversion
    float result = (2.0f * x) / C_RES_WIDTH - 1.0f;

    // Debug information
    std::cout << "convertXtoFloat called with x = " << x << std::endl;
    std::cout << "C_RES_WIDTH = " << C_RES_WIDTH << std::endl;
    std::cout << "Result = " << result << std::endl;

    // Check if the result is out of bounds
    if (result < -1.0f || result > 1.0f) {
        std::cout << "Warning: Result out of bounds: " << result << std::endl;
    }
    return result;
}

float convertYtoFloat(double y) {
    // Perform the conversion
    float result = 1.0f - (2.0f * y) / C_RES_HEIGHT;

    // Debug information
    std::cout << "convertYtoFloat called with y = " << y << std::endl;
    std::cout << "C_RES_HEIGHT = " << C_RES_HEIGHT << std::endl;
    std::cout << "Result = " << result << std::endl;

    // Check if the result is out of bounds
    if (result < -1.0f || result > 1.0f) {
        std::cout << "Warning: Result out of bounds: " << result << std::endl;
    }
    return result;
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

void Renderer::drawBackgroundTexture(GLuint textureID) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
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



