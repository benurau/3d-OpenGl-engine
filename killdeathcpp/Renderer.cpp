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


void Renderer::draw(const Mesh& mesh, Material& material){
    if (!material.shader) {
        std::cerr << "[Draw] Error: Material has no shader.\n";
        return;
    }

    //std::cout << "[Draw] Using shader ID: " << material.shader->ID << std::endl;

    mesh.orientation.updateShader(material.shader);
    checkGLError("setMat4(model)");

    material.apply();
    checkGLError("material.apply");

    //std::cout << "[Draw] Binding VAO ID: " << mesh.vao << std::endl;
    glBindVertexArray(mesh.vao);
    checkGLError("glBindVertexArray");
    //material.shader->PrintDebugUniforms();

    if (!mesh.indices.empty()) {
        //std::cout << "[Draw] Drawing with glDrawElements, count: " << mesh.indices.size() << std::endl;
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        checkGLError("glDrawElements");
    }
    else {
        //std::cout << "[Draw] Drawing with glDrawArrays, count: " << mesh.totalVerticles << std::endl;
        glDrawArrays(GL_TRIANGLES, 0, mesh.totalVerticles);
        checkGLError("glDrawArrays");
    }

    glBindVertexArray(0);
    //std::cout << "[Draw] Unbound VAO." << std::endl;


    glActiveTexture(GL_TEXTURE0);
    checkGLError("activatetexture0");
    //std::cout << "[Draw] Reset active texture to GL_TEXTURE0." << std::endl;
}











