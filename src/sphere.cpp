#include "primitives.hpp"
#define PI 3.14159265359f
Mesh Primitives::makeSphere(float radius, int sectorCount, int stackCount) {
    Mesh m;

    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = PI / 2 - i * PI / stackCount; // from pi/2 to -pi/2
        float xy = radius * cosf(stackAngle);             // r * cos(u)
        float z = radius * sinf(stackAngle);              // r * sin(u)

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * 2 * PI / sectorCount; // from 0 to 2pi
            float x = xy * cosf(sectorAngle);           // r * cos(u) * cos(v)
            float y = xy * sinf(sectorAngle);           // r * cos(u) * sin(v)
            m.positions.push_back(glm::vec3(x, y, z));

            glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
            m.normals.push_back(normal);
        }
    }

    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1);     // beginning of current stack
        int k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                m.indices.push_back(k1);
                m.indices.push_back(k2);
                m.indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1)) {
                m.indices.push_back(k1 + 1);
                m.indices.push_back(k2);
                m.indices.push_back(k2 + 1);
            }
        }
    }

    m.upload();
    return m;
}