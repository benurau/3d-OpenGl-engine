#include "openglHelpers.h"
#include <sstream>
#include "stb_image.h"
#include "shader.h"

std::string getGLErrorString(GLenum err) {
    switch (err) {
    case GL_NO_ERROR: return "GL_NO_ERROR";
    case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
    case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
    default: {
        std::ostringstream ss;
        ss << "Unknown error (0x" << std::hex << err << ")";
        return ss.str();
    }
    }
}

std::string checkGLError(const std::string& label) {
    GLenum err;
    std::ostringstream errorMessages;
    while ((err = glGetError()) != GL_NO_ERROR) {
        errorMessages << "[OpenGL Error] " << getGLErrorString(err)
            << " (" << err << ") after: " << label << "\n";
    }
    return errorMessages.str();
}


GLuint generateVBO(const std::vector<Vertex>& vertices) {
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    checkGLError("generatevbo");
    return vbo;
}

GLuint generateVAO() {
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    checkGLError("generatevao");
    return vao;
}

GLuint generateIBO(const std::vector<GLuint>& indices) {
    assert(indices.size() > 0, "objectconstructor3 indices");
    GLuint IBO = 0;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
    checkGLError("generateibo");
    return IBO;
}

void bindToVao(GLuint vao, int vertexArray, int vecSize, int stride, int offset) {
    glBindVertexArray(vao);
    glEnableVertexAttribArray(vertexArray);
    glVertexAttribPointer(vertexArray, vecSize, GL_FLOAT, GL_FALSE, stride, (void*)offset);
    checkGLError("bindtovao");
}

GLuint create2DBitMapTexture(const char* filepath) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
    if (data == NULL) {
        printf("Error loading image: %s\n", stbi_failure_reason());
        return 1;
    }

    GLenum format = 0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

    GLuint textureID = 0;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return textureID;
}

