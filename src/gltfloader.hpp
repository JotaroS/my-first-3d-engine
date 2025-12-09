#pragma once
#include <tinygltf/tiny_gltf.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include "joint.hpp"
#include "mesh.hpp"
#include <vector>
struct GltfSkinData{
    Mesh mesh;
    std::vector<Joint> joints;
    std::vector<glm::ivec4> joints0;
    std::vector<glm::vec4> weights0;
};

GltfSkinData loadGltfSkin(const std::string& path);