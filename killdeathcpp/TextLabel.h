#pragma once
#include "UIElement.h"
#include <string>

class TextLabel : public UIElement {
public:
    TextLabel(float x, float y, const std::string& text, float scale = 1.0f, const glm::vec3& color = glm::vec3(1.0f))
        : UIElement(x, y, 0, 0), text(text), scale(scale), color(color) {}

    void Render(TextRenderer& textRenderer) override {
        textRenderer.RenderText(text, position.x, position.y, scale, color);
    }

    void SetText(const std::string& t) { text = t; }
    void SetScale(float s) { scale = s; }
    void SetColor(const glm::vec3& c) { color = c; }

    const std::string& GetText() const { return text; }
    float GetScale() const { return scale; }
    const glm::vec3& GetColor() const { return color; }

private:
    std::string text;
    float scale;
    glm::vec3 color;
};
