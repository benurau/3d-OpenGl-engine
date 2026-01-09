#ifndef MATERIAL_H
#define MATERIAL_H

#include "shader.h"
#include <map>

class Material {
public:
    Shader* shader;

    std::map<std::string, float> floatUniforms;
    std::map<std::string, glm::vec3> vec3Uniforms;
    std::map<std::string, glm::vec4> vec4Uniforms;
    std::map<std::string, glm::mat4> mat4Uniforms;
    std::map<std::string, Texture> textureUniforms;

    Material() = default;
    Material(Shader* shader);
    void bindTextures() const;
    void apply(); 
    void ValidateUniforms() const;
};

#endif