#include "Material.h"
#include <unordered_set>


Material::Material(Shader* shader) {
    this->shader = shader;
}

void Material::bindTextures() const
{
    int i = 0;
    GLint program = shader ? shader->ID : 0;

    for (const auto& pair : textureUniforms) {
        const std::string& uniformName = pair.first;
        const Texture texture = pair.second;
        GLint loc = shader->getUniformLocation(uniformName);
        if (loc == -1) continue;
        glActiveTexture(GL_TEXTURE0 + i);
        checkGLError("material.bindtextures() glActiveTexture " + std::to_string(i));
        glUniform1i(loc, i);
        checkGLError("material.bindtextures() glUniform1i " + uniformName);
        glBindTexture(GL_TEXTURE_2D, texture.id);
        checkGLError("material.bindtextures() glBindTexture " + uniformName);
        ++i;
    }
}


void Material::apply() {
    shader->use();
    for (const auto& [name, value] : floatUniforms) {
        shader->setFloat(name, value);
        checkGLError("material.apply() shader setfloat variable " + name);
    }
    for (const auto& [name, value] : vec3Uniforms) {
        shader->setVec3(name, value);
        checkGLError("material.apply() shader setvec3 variable " + name);
    }
    for (const auto& [name, value] : vec4Uniforms) {
        shader->setVec4(name, value);
        checkGLError("material.apply() shader setvec4 variable " + name);
    }
    for (const auto& [name, value] : mat4Uniforms) {
        shader->setMat4(name, value);
        checkGLError("material.apply() shader setmat4 variable " + name);
    }
    bindTextures();
}

void Material::ValidateUniforms() const {
    std::unordered_set<std::string> materialUniformNames;
    for (const auto& [name, _] : floatUniforms)   materialUniformNames.insert(name);
    for (const auto& [name, _] : vec3Uniforms)    materialUniformNames.insert(name);
    for (const auto& [name, _] : vec4Uniforms)    materialUniformNames.insert(name);
    for (const auto& [name, _] : mat4Uniforms)    materialUniformNames.insert(name);
    for (const auto& [name, _] : textureUniforms) materialUniformNames.insert(name);
    const auto& shaderUniforms = shader->uniformValues;

    for (const auto& [uniformName, uniformType] : shaderUniforms) {
        if (materialUniformNames.find(uniformName) == materialUniformNames.end() && uniformName != "model" && uniformName != "view" && uniformName != "projection") {
            std::cout << "Warning: Shader expects uniform '" << uniformName
                << "' of type " << uniformType << ", but it's missing from the material.\n";
        }
    }
};
