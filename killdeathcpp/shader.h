#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
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
        PrintActiveUniforms(ID);
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
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setMat4(const std::string& name, const glm::mat4& mat) const {
        GLint loc = glGetUniformLocation(ID, name.c_str());
        if (loc != -1) {
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
        }
        else {
            std::cerr << "[Shader] Uniform not found or inactive: " << name << std::endl;
        }
    }
    glm::mat4 getMat4(const GLchar* name) {
        return glGetUniformLocation(ID, name);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
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

    void PrintActiveUniforms(GLuint program) {
        GLint numUniforms = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

        std::cout << "=== Active Uniforms ===\n";

        for (int i = 0; i < numUniforms; ++i) {
            char name[256];
            GLsizei length;
            GLint size;
            GLenum type;

            glGetActiveUniform(program, i, sizeof(name), &length, &size, &type, name);

            GLint location = glGetUniformLocation(program, name);
            std::cout << "Uniform #" << i << ": " << name << ", Location: " << location << ", Type: " << type << "\n";

            if (location == -1) {
                std::cout << "   Warning: Uniform " << name << " is not active or optimized out.\n";
            }
        }

        std::cout << "========================\n";
    }

};
#endif