#include "TextRenderer.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_truetype.h"
#include "misc.h"

TextRenderer::TextRenderer(const std::string& fontPath, unsigned int fontSize)
    : shader("..\\shaders\\text.vs", "..\\shaders\\text.fs"), atlasTexture(0), vao(0), vbo(0), atlasWidth(512), atlasHeight(512)
{
    std::ifstream file(fontPath, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "[TextRenderer] Failed to open font: " << fontPath << std::endl;
        return;
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<unsigned char> fontBuffer(size);
    if (!file.read(reinterpret_cast<char*>(fontBuffer.data()), size)) {
        std::cerr << "[TextRenderer] Failed to read font file" << std::endl;
        return;
    }

    std::vector<unsigned char> bitmap(atlasWidth * atlasHeight, 0);
    stbtt_bakedchar cdata[96];

    int result = stbtt_BakeFontBitmap(fontBuffer.data(), 0, static_cast<float>(fontSize), bitmap.data(), atlasWidth, atlasHeight, 32, 96, cdata);
    if (result <= 0) {
        std::cerr << "[TextRenderer] stbtt_BakeFontBitmap failed (returned " << result << ")" << std::endl;
        return;
    }

    for (int i = 0; i < 96; i++) {
        char c = static_cast<char>(32 + i);
        stbtt_bakedchar& b = cdata[i];
        Character ch;
        ch.ax = b.xadvance;
        ch.ay = 0;
        ch.bw = static_cast<float>(b.x1 - b.x0);
        ch.bh = static_cast<float>(b.y1 - b.y0);
        ch.bl = b.xoff;
        ch.bt = b.yoff;
        ch.tx = static_cast<float>(b.x0) / atlasWidth;
        ch.ty = static_cast<float>(b.y0) / atlasHeight;
        ch.tx2 = static_cast<float>(b.x1) / atlasWidth;
        ch.ty2 = static_cast<float>(b.y1) / atlasHeight;
        characters[c] = ch;
    }

    glGenTextures(1, &atlasTexture);
    glBindTexture(GL_TEXTURE_2D, atlasTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasWidth, atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::cout << "[TextRenderer] Ready: " << fontPath << " (" << characters.size() << " glyphs, atlas " << atlasWidth << "x" << atlasHeight << ")" << std::endl;
}

TextRenderer::~TextRenderer() {
    if (atlasTexture) glDeleteTextures(1, &atlasTexture);
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
}

void TextRenderer::RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
    if (characters.empty() || atlasTexture == 0) return;

    shader.use();
    shader.setVec3("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlasTexture);
    shader.setInt("text", 0);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(C_RES_WIDTH), static_cast<float>(C_RES_HEIGHT), 0.0f);
    shader.setMat4("projection", projection);

    glBindVertexArray(vao);

    for (char c : text) {
        if (c == '\n') {
            y += 28.0f * scale;
            x = 0;
            continue;
        }
        auto it = characters.find(c);
        if (it == characters.end()) continue;
        Character& ch = it->second;

        float xpos = x + ch.bl * scale;
        float ypos = y + ch.bt * scale;
        float w = ch.bw * scale;
        float h = ch.bh * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h, ch.tx, ch.ty2 },
            { xpos,     ypos,     ch.tx, ch.ty },
            { xpos + w, ypos,     ch.tx2, ch.ty },
            { xpos,     ypos + h, ch.tx, ch.ty2 },
            { xpos + w, ypos,     ch.tx2, ch.ty },
            { xpos + w, ypos + h, ch.tx2, ch.ty2 }
        };

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += ch.ax * scale;
    }

    glBindVertexArray(0);
}
