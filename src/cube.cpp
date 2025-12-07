#include "primitives.hpp"

Mesh Primitives::makeCube(float s) {
    Mesh m;

    glm::vec3 p[] = {
        {-s,-s,-s}, { s,-s,-s}, { s, s,-s}, {-s, s,-s},
        {-s,-s, s}, { s,-s, s}, { s, s, s}, {-s, s, s}
    };
    glm::vec3 n[] = {
        {0,0,-1},{0,0,-1},{0,0,-1},{0,0,-1},
        {0,0,1},{0,0,1},{0,0,1},{0,0,1}
    };

    unsigned int idx[] = {
        0,1,2, 2,3,0,  // front
        4,5,6, 6,7,4,  // back
        0,4,7, 7,3,0,  // left
        1,5,6, 6,2,1,  // right
        3,2,6, 6,7,3,  // top
        0,1,5, 5,4,0   // bottom
    };

    m.positions.assign(p, p+8);
    m.normals.assign(n, n+8);
    m.indices.assign(idx, idx + 36);

    m.upload();
    return m;
}
