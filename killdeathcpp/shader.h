#ifndef SHADER_H
#define SHADER_H

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "openglHelpers.h"



class Shader
{
public:
    unsigned int ID;
    int shaderExists;

    Shader() {}
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        ID = 0;
        shaderExists = 1;
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream Objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        SetupDebugUniformMap(ID);
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        if (ID == 0) {
            std::cerr << "[Shader::use] ERROR: Shader ID is 0, cannot use program." << std::endl;
            return;
        }
        glUseProgram(ID);
        checkGLError("shader use in shader .h");
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const
    {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        if (loc != -1) {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
            uniformValues[name] = value ? "true" : "false";
        }
        else {
            std::cerr << "[Shader] Uniform not found or inactive: " << name << std::endl;
        }
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        if (loc != -1) {
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
            uniformValues[name] = std::to_string(value);
        }
        else {
            std::cerr << "[Shader] Uniform not found or inactive: " << name << std::endl;
        }
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        if (loc != -1) {
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
            uniformValues[name] = std::to_string(value);
        }
        else {
            std::cerr << "[Shader] Uniform not found or inactive: " << name << std::endl;
        }
    }
    void setVec3(const std::string& name, const glm::vec3& value) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        if (loc != -1) {
            glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
            uniformValues[name] = glm::to_string(value);
        }
        else {
            std::cerr << "[Shader] Uniform not found or inactive: " << name << std::endl;
        }   
    }
    void setMat4(const std::string& name, const glm::mat4& mat) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        if (loc != -1) {
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
            uniformValues[name] = glm::to_string(mat);
        }
        else {
            std::cerr << "[Shader] Uniform not found or inactive: " << name << std::endl;
        }
    }
    glm::mat4 getMat4(const GLchar* name) {
        return glGetUniformLocation(ID, name);
    }

    void PrintDebugUniforms() const {
        std::cout << "=== Debug Uniform Values ===\n";
        for (const auto& pair : uniformValues) {
            std::cout << pair.first << " = " << pair.second << "\n";
        }
        std::cout << "============================\n";
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    mutable std::unordered_map<std::string, std::string> uniformValues;
    void checkCompileErrors(GLuint shader, const std::string& type)
    {
        GLint success;
        GLchar infoLog[1024];

        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                    << infoLog << "\n -- --------------------------------------------------- -- "
                    << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                    << infoLog << "\n -- --------------------------------------------------- -- "
                    << std::endl;
            }
        }
    }

    void SetupDebugUniformMap(GLuint program) {
        GLint numUniforms = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

        for (int i = 0; i < numUniforms; ++i) {
            char name[256];
            GLsizei length;
            GLint size;
            GLenum type;

            glGetActiveUniform(program, i, sizeof(name), &length, &size, &type, name);
            uniformValues[name] = "empty"; // initialize debug tracking
        }
    }


};
#endif