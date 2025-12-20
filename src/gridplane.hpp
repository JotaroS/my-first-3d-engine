#pragma once

// simple grid plnae in XZ plane
#include "mesh.hpp"

namespace GridPlane {
    Mesh makeGridPlane(float size = 10.0f, int divisions = 10);
};