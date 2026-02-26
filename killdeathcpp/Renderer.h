#ifndef RENDERER_H
#define RENDERER_H



#include <glad/glad.h>    
#include <GLFW/glfw3.h>
#include "Mesh.h"
#include "ObjectOrientation.h"
#include "tinyModel.h"
#include "Colissions.h"



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
    void drawLine(const glm::vec3& a, const glm::vec3& b, const glm::vec3& color, const glm::mat4& viewProj, Shader& shader);
    void drawCircle(const glm::vec3& center, const glm::vec3& normal, float radius, const glm::vec3& color, const glm::mat4& viewProj, Shader& shader);
    void drawAABB(const AABB& box, const glm::mat4& viewProj, const glm::vec3& color, Shader& shader);
    void drawCapsule(const CapsuleWorldLoc& c, const glm::mat4& viewProj, const glm::vec3& color, Shader& shader);

private:
    GLFWwindow* window;
};
#endif