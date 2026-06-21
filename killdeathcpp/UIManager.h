#pragma once
#include "UIScreen.h"
#include <memory>



class UIManager {
public:
    void SetScreen(UIScreen* screen) {
        currentScreen.reset(screen);
    }

    void Update(float dt) {
        if (currentScreen) currentScreen->Update(dt);
    }

    void Render(TextRenderer& textRenderer) {
        if (currentScreen) currentScreen->Render(textRenderer);
    }

    bool OnMouseClick(float mx, float my) {
        return currentScreen ? currentScreen->OnMouseClick(mx, my) : false;
    }

    void OnMouseMove(float mx, float my) {
        if (currentScreen) currentScreen->OnMouseMove(mx, my);
    }

    UIScreen* GetCurrentScreen() const { return currentScreen.get(); }

private:
    std::unique_ptr<UIScreen> currentScreen;
};
