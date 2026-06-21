#pragma once
#include <string>
#include <map>
#include <glm/glm.hpp>
#include "shader.h"

struct Character {
    float ax, ay;
    float bw, bh;
    float bl, bt;
    float tx, ty;
    float tx2, ty2;
};

class TextRenderer {
public:
    TextRenderer(const std::string& fontPath, unsigned int fontSize = 48);
    void RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color);
    glm::vec2 MeasureText(const std::string& text, float scale);

private:
    std::map<char, Character> characters;
    Shader shader;
    GLuint vao, vbo;
    GLuint atlasTexture;
    int atlasWidth, atlasHeight;
};
