#include "Material.h"
#include <unordered_set>


Material::Material(Shader* shader) {
    this->shader = shader;
}

void Material::bindTextures() const {
    int i = 0;
    for (const auto& pair : textureUniforms) {
        int location = glGetUniformLocation(shader->ID, "useTexture");
        if (location != -1) 
            shader->setBool("useTexture", true);

        const std::string& uniformName = pair.first;
        const Texture* texture = pair.second;

        if (!texture) {
            std::cerr << "[Warn] Null texture pointer for uniform: " << uniformName << std::endl;
            continue;
        }
        glActiveTexture(GL_TEXTURE0 + i);
        checkGLError("glActiveTexture " + std::to_string(i));

        GLint loc = glGetUniformLocation(shader->ID, uniformName.c_str());
        if (loc != -1) {
            glUniform1i(loc, i);
            checkGLError("glUniform1i " + uniformName);
        }
        else {
            std::cout << "[Warn] Uniform not found or inactive: " << uniformName << std::endl;
        }

        glBindTexture(GL_TEXTURE_2D, texture->id);
        checkGLError("glBindTexture " + uniformName);
        ++i;
    }
}

void Material::apply() {
    shader->use();

    for (const auto& [name, value] : floatUniforms)
        shader->setFloat(name, value);

    for (const auto& [name, value] : vec3Uniforms)
        shader->setVec3(name, value);

    for (const auto& [name, value] : mat4Uniforms)
        shader->setMat4(name, value);

    bindTextures();
}

void Material::ValidateUniforms() const {
    std::unordered_set<std::string> materialUniformNames;
    for (const auto& [name, _] : floatUniforms)   materialUniformNames.insert(name);
    for (const auto& [name, _] : vec3Uniforms)    materialUniformNames.insert(name);
    for (const auto& [name, _] : mat4Uniforms)    materialUniformNames.insert(name);
    for (const auto& [name, _] : textureUniforms) materialUniformNames.insert(name);
    const auto& shaderUniforms = shader->uniformValues;

    for (const auto& [uniformName, uniformType] : shaderUniforms) {
        if (materialUniformNames.find(uniformName) == materialUniformNames.end()) {
            std::cout << "Warning: Shader expects uniform '" << uniformName
                << "' of type " << uniformType << ", but it's missing from the material.\n";
        }
    }
    for (const std::string& materialName : materialUniformNames) {
        if (shaderUniforms.find(materialName) == shaderUniforms.end()) {
            std::cout << "Notice: Material provides extra uniform '" << materialName
                << "', but it's not used in the shader.\n";
        }
    }
}