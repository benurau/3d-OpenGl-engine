#pragma once
#include "UIScreen.h"
#include "TextLabel.h"
#include "Button.h"

class PauseScreen : public UIScreen {
public:
    PauseScreen(std::function<void()> onResume,
                std::function<void()> onOptions,
                std::function<void()> onQuitToMenu) {
        float cx = 412.0f;
        float bw = 200.0f, bh = 50.0f;

        AddElement<TextLabel>(380.0f, 150.0f, "PAUSED", 2.0f, glm::vec3(0.2f, 0.6f, 1.0f));

        AddElement<Button>(cx, 280.0f, bw, bh, "Resume", std::move(onResume));
        AddElement<Button>(cx, 360.0f, bw, bh, "Options", std::move(onOptions));
        AddElement<Button>(cx, 440.0f, bw, bh, "Quit to Menu", std::move(onQuitToMenu));
    }
};
