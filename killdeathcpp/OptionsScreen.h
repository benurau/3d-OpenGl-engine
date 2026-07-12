#pragma once
#include "UIScreen.h"
#include "TextLabel.h"
#include "Button.h"

class OptionsScreen : public UIScreen {
public:
    OptionsScreen(std::function<void()> onBack) {
        float cx = 412.0f;

        AddElement<TextLabel>(330.0f, 150.0f, "OPTIONS", 1.5f, glm::vec3(1.0f, 0.8f, 0.2f));

        AddElement<TextLabel>(300.0f, 280.0f, "Sound: ON", 1.0f, glm::vec3(0.8f, 0.8f, 0.8f));
        AddElement<TextLabel>(300.0f, 330.0f, "Sensitivity: 1.0", 1.0f, glm::vec3(0.8f, 0.8f, 0.8f));

        AddElement<Button>(412.0f, 500.0f, 200.0f, 50.0f, "Back", std::move(onBack));
    }
};
