#ifndef RENDERER_H
#define RENDERER_H



#include <glad/glad.h>    
#include <GLFW/glfw3.h>
#include "Mesh.h"
#include "ObjectOrientation.h"
#include "tinyModel.h"



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
    std::vector<Material> materials;
    Renderer(GLFWwindow* window);

    void clear();
    void swapBuffers();
    void drawModel(tinyModel& model, ObjectOrientation& orientation);
    Material ConvertGLTFMaterialToMaterial(const GLTFMaterialGPU& src, Shader* shader);
    void draw(Mesh& mesh, ObjectOrientation& orientation, Material& material);

private:
    GLFWwindow* window;
};
#endif