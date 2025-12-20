#include "gridplane.hpp"
#include <glad/glad.h>

Mesh GridPlane::makeGridPlane(float size, int divisions) {
    Mesh m;

    float halfSize = size / 2.0f;
    float divisionSize = size / divisions;

    for (int i = 0; i <= divisions; ++i) {
        float coord = -halfSize + i * divisionSize;

        // Lines along X axis
        m.positions.push_back(glm::vec3(-halfSize, 0.0f, coord));
        m.positions.push_back(glm::vec3(halfSize, 0.0f, coord));
        m.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        m.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

        // Lines along Z axis
        m.positions.push_back(glm::vec3(coord, 0.0f, -halfSize));
        m.positions.push_back(glm::vec3(coord, 0.0f, halfSize));
        m.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        m.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // Create indices
    for (int i = 0; i < divisions + 1; ++i) {
        m.indices.push_back(i * 4 + 0);
        m.indices.push_back(i * 4 + 1);
        m.indices.push_back(i * 4 + 2);
        m.indices.push_back(i * 4 + 3);
    }

    m.upload();
    return m;
}