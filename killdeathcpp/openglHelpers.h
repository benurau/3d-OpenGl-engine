#ifndef OPENGLHELPERS_H
#define OPENGLHELPERS_H

#include <glad/glad.h>	
#include "Geometry3D.h"


std::string getGLErrorString(GLenum err);
void checkGLError(const std::string& label);
GLuint generateVBO(const std::vector<Vertex>& vertices);
GLuint generateVAO();
GLuint generateIBO(const std::vector<GLuint>& indices);
void bindToVao(GLuint vertexArray, GLint vecSize, GLsizei stride, size_t offset);
GLuint create2DBitMapTexture(const char* filepath);

#endif
