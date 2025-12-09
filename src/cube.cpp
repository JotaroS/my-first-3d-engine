#include "primitives.hpp"

Mesh Primitives::makeCube(float s) {
    Mesh m;

    // Each face needs its own vertices with proper normals
    m.positions = {
        // Front face (z = s)
        {-s,-s, s}, { s,-s, s}, { s, s, s}, {-s, s, s},
        // Back face (z = -s)
        { s,-s,-s}, {-s,-s,-s}, {-s, s,-s}, { s, s,-s},
        // Left face (x = -s)
        {-s,-s,-s}, {-s,-s, s}, {-s, s, s}, {-s, s,-s},
        // Right face (x = s)
        { s,-s, s}, { s,-s,-s}, { s, s,-s}, { s, s, s},
        // Top face (y = s)
        {-s, s, s}, { s, s, s}, { s, s,-s}, {-s, s,-s},
        // Bottom face (y = -s)
        {-s,-s,-s}, { s,-s,-s}, { s,-s, s}, {-s,-s, s}
    };

    m.normals = {
        // Front face
        {0,0,1}, {0,0,1}, {0,0,1}, {0,0,1},
        // Back face
        {0,0,-1}, {0,0,-1}, {0,0,-1}, {0,0,-1},
        // Left face
        {-1,0,0}, {-1,0,0}, {-1,0,0}, {-1,0,0},
        // Right face
        {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0},
        // Top face
        {0,1,0}, {0,1,0}, {0,1,0}, {0,1,0},
        // Bottom face
        {0,-1,0}, {0,-1,0}, {0,-1,0}, {0,-1,0}
    };

    m.indices = {
        0,1,2,   2,3,0,    // Front
        4,5,6,   6,7,4,    // Back
        8,9,10,  10,11,8,  // Left
        12,13,14, 14,15,12, // Right
        16,17,18, 18,19,16, // Top
        20,21,22, 22,23,20  // Bottom
    };

    m.upload();
    return m;
}
