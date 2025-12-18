#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "renderobject.hpp"
#include "light.hpp"

// Forward declaration
class Scene {
public:
    glm::vec3 backgroundColor {0.1f, 0.1f, 0.12f};

    std::vector<Light> lights;
    std::vector<RenderObject*> renderObjects;

    void addRenderObject(RenderObject* obj) {
        renderObjects.push_back(obj);
    }

    void addLight(glm::vec3 pos,
                  glm::vec3 col = {1.0f, 0.95f, 0.9f}, float intensity = 1.0f)
    {
        Light light(pos, col, intensity);
        lights.push_back(light);
    }

    void setBackground(glm::vec3 col) {
        backgroundColor = col;
    }
};
