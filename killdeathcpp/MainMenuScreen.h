#pragma once
#include "UIScreen.h"
#include "TextLabel.h"
#include "Button.h"

class MainMenuScreen : public UIScreen {
public:
    MainMenuScreen(std::function<void()> onStartGame,
                   std::function<void()> onOptions,
                   std::function<void()> onQuit) {
        float cx = 412.0f;
        float bw = 200.0f, bh = 50.0f;

        AddElement<TextLabel>(362.0f, 150.0f, "KILL DEATH CPP", 2.0f, glm::vec3(1.0f, 0.2f, 0.2f));

        AddElement<Button>(cx, 280.0f, bw, bh, "Start Game", std::move(onStartGame));
        AddElement<Button>(cx, 360.0f, bw, bh, "Options", std::move(onOptions));
        AddElement<Button>(cx, 440.0f, bw, bh, "Quit", std::move(onQuit));
    }
};
