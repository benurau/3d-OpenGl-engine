#pragma once
#include <glm/glm.hpp>
#include "TextRenderer.h"


class UIElement {
public:
    UIElement(float x, float y, float w, float h)
        : position(x, y), size(w, h) {}
    virtual ~UIElement() = default;

    virtual void Update(float dt) {}
    virtual void Render(TextRenderer& textRenderer) = 0;

    virtual bool OnMouseClick(float mx, float my) {
        return mx >= position.x && mx <= position.x + size.x && my >= position.y && my <= position.y + size.y;
    }

    virtual bool OnMouseMove(float mx, float my) { return false; }

    void SetPosition(float x, float y) { position.x = x; position.y = y; }
    void SetSize(float w, float h) { size.x = w; size.y = h; }

    glm::vec2 GetPosition() const { return position; }
    glm::vec2 GetSize() const { return size; }

protected:
    glm::vec2 position;
    glm::vec2 size;
};
