#pragma once
#include "component.hpp"

class Light : public Component {
public:
    glm::vec3 color = glm::vec3(1.0f); // Default to white light
    float intensity = 1.0f; // Default intensity
    Light(glm::vec3 col = glm::vec3(1.0f), float inten = 1.0f)
        : color(col), intensity(inten) {}
    void update() override {
        // Light might not need per-frame updates
    }

    void draw(Renderer& renderer) override {
        // Light might not have a direct draw call
        // It could set some global shader uniforms instead
    }
};