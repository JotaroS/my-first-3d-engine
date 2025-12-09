#pragma once
#include "mesh.hpp"

namespace Primitives {
    Mesh meshCube(float s = 1.0f);
    Mesh makeCube(float s = 1.0f);
    Mesh makeSphere(float radius = 1.0f, int sectorCount = 36, int stackCount = 18);
};
