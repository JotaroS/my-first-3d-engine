#pragma once
#include "glm/gtc/quaternion.hpp"
#include <glm/glm.hpp>
struct Joint {
    int parent = -1;
    glm::mat4 local;      // local bind transform (given by glTF)
    glm::mat4 invBind;    // inverse bind matrix (given by glTF)
    glm::mat4 world;      // computed each frame in FK

    void rotateLocal(const glm::quat& q) {
        local = local * glm::mat4_cast(q);
    }
    void rotateAxisLocal(const glm::vec3& axis, float angleRad, bool degrees = false) {
        if (degrees) {
            angleRad = glm::radians(angleRad);
        }
        local = local * glm::rotate(glm::mat4(1.0f), angleRad, axis);
    }
    void rotateEulerLocal(const glm::vec3& eulerRad, bool degrees = false) {
        glm::vec3 euler = eulerRad;
        if (degrees) {
            euler = glm::radians(eulerRad);
        }
        local = local * glm::mat4_cast(glm::quat(euler));
    }
};