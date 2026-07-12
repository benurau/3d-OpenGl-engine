#pragma once
#include "UIElement.h"
#include <vector>
#include <memory>

class UIScreen {
public:
    virtual ~UIScreen() = default;

    virtual void Update(float dt) {
        for (auto& element : elements) {
            element->Update(dt);
        }
    }

    virtual void Render(TextRenderer& textRenderer) {
        for (auto& element : elements) {
            element->Render(textRenderer);
        }
    }

    virtual bool OnMouseClick(float mx, float my) {
        for (auto it = elements.rbegin(); it != elements.rend(); ++it) {
            if ((*it)->OnMouseClick(mx, my)) return true;
        }
        return false;
    }

    virtual void OnMouseMove(float mx, float my) {
        for (auto& element : elements) {
            element->OnMouseMove(mx, my);
        }
    }

    template<typename T, typename... Args>
    T& AddElement(Args&&... args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *ptr;
        elements.push_back(std::move(ptr));
        return ref;
    }

protected:
    std::vector<std::unique_ptr<UIElement>> elements;
};
