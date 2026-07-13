#pragma once
#include "TextLabel.h"
#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include "misc.h"

class Button : public UIElement {
public:
    Button(float x, float y, float w, float h, const std::string& label,
           std::function<void()> onClick = nullptr,
           const glm::vec3& normalColor = glm::vec3(0.3f, 0.3f, 0.5f),
           const glm::vec3& textColor = glm::vec3(1.0f),
           float textScale = 1.0f)
        : UIElement(x, y, w, h)
        , label(label)
        , onClick(std::move(onClick))
        , normalColor(normalColor)
        , textColor(textColor)
        , textScale(textScale)
        , vao(0), vbo(0), ebo(0) {
        InitQuad();
    }

    ~Button() {
        if (vao) glDeleteVertexArrays(1, &vao);
        if (vbo) glDeleteBuffers(1, &vbo);
        if (ebo) glDeleteBuffers(1, &ebo);
    }

    void Render(TextRenderer& textRenderer) override {
        
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glm::vec2 textSize = textRenderer.MeasureText(label, textScale);
        float textX = position.x + (size.x - textSize.x) / 2.0f;
        float textY = position.y + (size.y - textSize.y) / 2.0f;
        textRenderer.RenderText(label, textX, textY, textScale, textColor);
    }

    bool OnMouseClick(float mx, float my) override {
        if (!UIElement::OnMouseClick(mx, my)) return false;
        if (onClick) onClick();
        return true;
    }

    void SetLabel(const std::string& l) { label = l; }
    void SetColors(const glm::vec3& normal, const glm::vec3& hover) {
        normalColor = normal;
    }

private:
    void InitQuad() {
        float vertices[] = {
            0.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f
        };
        unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    std::string label;
    std::function<void()> onClick;
    glm::vec3 normalColor;
    glm::vec3 textColor;
    float textScale;

    GLuint vao, vbo, ebo;
};
