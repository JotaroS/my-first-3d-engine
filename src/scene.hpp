#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "renderobject.hpp"
#include "mesh.hpp"

struct Light {
    glm::vec3 position;
    glm::vec3 color;
};

class Scene {
public:
    glm::vec3 backgroundColor {0.1f, 0.1f, 0.12f};

    std::vector<Light> lights;
    std::vector<Mesh*> objects;

    void add(Mesh& obj) {
        objects.push_back(&obj);
    }

    void addLight(glm::vec3 pos,
                  glm::vec3 col = {1.0f, 0.95f, 0.9f})
    {
        lights.push_back({pos, col});
    }

    void setBackground(glm::vec3 col) {
        backgroundColor = col;
    }
};
