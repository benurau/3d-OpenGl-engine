#ifndef MATERIAL_H
#define MATERIAL_H

#include "shader.h"

class Material {
public:
    Shader* shader;

    std::unordered_map<std::string, float> floatUniforms;
    std::unordered_map<std::string, glm::vec3> vec3Uniforms;
    std::unordered_map<std::string, glm::mat4> mat4Uniforms;
    std::unordered_map<std::string, Texture*> textureUniforms;

    Material(Shader* shader);
    void bindTextures() const;
    void apply(); 
    void ValidateUniforms() const;
};

#endif