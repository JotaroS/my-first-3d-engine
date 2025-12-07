#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform {
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};

    Transform* parent = nullptr;

    glm::mat4 localMatrix() const {
        glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 R = glm::mat4_cast(rotation);
        glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
        glm::mat4 localMatrix = T * R * S;
        return localMatrix;
    }

    glm::mat4 worldMatrix() const {
        if (parent) {
            return parent->worldMatrix() * localMatrix();
        } else {
            return localMatrix();
        }
    }
};