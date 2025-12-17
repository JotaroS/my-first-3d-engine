#pragma once
#include "component.hpp"
#include "renderer.hpp"

class AmbientLight : public Component {
public:
    glm::vec3 color = glm::vec3(1.0f); // Default to white light
    float intensity = 1.0f; // Default intensity
    void update() override {
        // Ambient light might not need per-frame updates
    }

    void draw(Renderer& renderer) override {
        // Ambient light might not have a direct draw call
        // It could set some global shader uniforms instead
    }
};