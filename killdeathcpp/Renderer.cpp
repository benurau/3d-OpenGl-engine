#include "renderer.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "misc.h"



Renderer::Renderer(GLFWwindow* window) : window(window){}
void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::swapBuffers() {
    glfwSwapBuffers(window);
}

GLuint Renderer::create2DBitMapTexture(const char* filepath) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
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
    shaders["guad"] = Shader("..\\shaders\\guad_vs.txt", "..\\shaders\\rainbow_fs.txt");
    shaders["line"] = Shader("..\\shaders\\line_vs.txt", "..\\shaders\\line_fs.txt");
    shaders["guadtexture"] = Shader("..\\shaders\\background_vs.txt", "..\\shaders\\background_fs.txt");
    shaders["guad3d"] = Shader("..\\shaders\\guad3d_vs.txt", "..\\shaders\\guad3d_fs.txt");
}

void Renderer::drawLine(Object object) {
    object.shader.use();
    glBindVertexArray(object.vao);
    glDrawArrays(GL_LINES, 0, 2);
}

//void Renderer::drawFixedLine(Object object, float length) {
//    float dirX = points[3] - points[0];
//    float dirY = points[4] - points[1];;
//    float distance = sqrt(dirX * dirX + dirY * dirY);
//
//    if (distance == 0) {
//        return;
//    }
//    float normDirX = dirX / distance;
//    float normDirY = dirY / distance;
//    float finalX = points[0] + normDirX * length;
//    float finalY = points[1] + normDirY * length;
//
//    GLfloat newPoints[] =
//    {
//        points[0], points[1], points[2],
//        finalX, finalY, points[5]
//    };
//
//    drawLine(vao , shader , newPoints, size);
//}

void Renderer::drawObject(Object object) {
    //std::cout << object.shader.ID<< "id \n";
    //std::cout << object.vao<< "vao \n";
    //std::cout << object.textureID<< "textureID \n";
    //std::cout << object.totalVerticles << "verticles\n";
    object.shader.use();
    if (object.textureID > 0) {
        //std::cout << "texture";
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, object.textureID);
    }
    glBindVertexArray(object.vao);
    if (object.ibo > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.ibo);
        glDrawElements(GL_TRIANGLES, object.totalVerticles, GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, object.totalVerticles);
    }
}





