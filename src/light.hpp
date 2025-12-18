#pragma once
#include "component.hpp"
#include <glm/glm.hpp>

class Light : public Component {
public:
    glm::vec3 color = glm::vec3(1.0f); // Default to white light
    glm::vec3 position = glm::vec3(0.0f);
    float intensity = 1.0f; // Default intensity
    Light(glm::vec3 col = glm::vec3(1.0f), float intensity = 1.0f)
        : color(col), intensity(intensity) {}
    Light(glm::vec3 pos, glm::vec3 col = glm::vec3(1.0f), float intensity = 1.0f)
        : position(pos), color(col), intensity(intensity) {}
    void update() override {
        // Light might not need per-frame updates
    }

    void draw(Renderer& renderer) override {
        // Light might not have a direct draw call
        // It could set some global shader uniforms instead
    }
};