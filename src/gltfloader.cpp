// This file provides the implementation of tinygltf
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "gltfloader.hpp"
#include <iostream>
#include <map>


// GLTF 読み込み
GltfSkinData loadGltfSkin(const std::string& path) {
    std::cout << "loadGltfSkin called with path: " << path << std::endl;
    std::cout.flush();
    
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err, warn;

    // Check if file is binary (.glb) or ASCII (.gltf)
    bool ret;
    if (path.find(".glb") != std::string::npos) {
        std::cout << "Loading as binary GLB file..." << std::endl;
        ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);
    } else {
        std::cout << "Loading as ASCII GLTF file..." << std::endl;
        ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
    }
    
    if (!warn.empty()) {
        std::cerr << "GLTF Warning: " << warn << std::endl;
        std::cerr.flush();
    }
    
    if (!err.empty()) {
        std::cerr << "GLTF Error: " << err << std::endl;
        std::cerr.flush();
    }
    
    if (!ret) {
        std::cerr << "Failed to load GLTF file: " << path << std::endl;
        std::cerr.flush();
        throw std::runtime_error("Failed to load GLTF file");
    }
    
    std::cout << "File loaded, checking meshes..." << std::endl;
    
    if (model.meshes.empty()) {
        std::cerr << "No meshes found in GLTF file" << std::endl;
        std::cerr.flush();
        throw std::runtime_error("No meshes found");
    }

    GltfSkinData out;

    // ---- 1. Mesh の POSITION / NORMAL / INDICES 読み取り ----
    const auto& mesh = model.meshes[0];
    const auto& prim = mesh.primitives[0];

    auto readVec3 = [&](const tinygltf::Accessor& acc) {
        std::vector<glm::vec3> v;
        v.resize(acc.count);
        const auto& bv = model.bufferViews[acc.bufferView];
        const auto& buf = model.buffers[bv.buffer];
        const float* p = reinterpret_cast<const float*>(&buf.data[bv.byteOffset]);
        for (int i = 0; i < acc.count; i++) {
            v[i] = glm::vec3(p[i*3+0], p[i*3+1], p[i*3+2]);
        }
        return v;
    };

    out.mesh.positions = readVec3(model.accessors[prim.attributes.at("POSITION")]);
    out.mesh.normals   = readVec3(model.accessors[prim.attributes.at("NORMAL")]);

    // indices
    {
        const auto& acc = model.accessors[prim.indices];
        const auto& bv  = model.bufferViews[acc.bufferView];
        const auto& buf = model.buffers[bv.buffer];

        out.mesh.indices.resize(acc.count);

        if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
            const uint16_t* p = reinterpret_cast<const uint16_t*>(&buf.data[bv.byteOffset]);
            for (int i = 0; i < acc.count; i++) out.mesh.indices[i] = p[i];
        } else {
            const uint32_t* p = reinterpret_cast<const uint32_t*>(&buf.data[bv.byteOffset]);
            for (int i = 0; i < acc.count; i++) out.mesh.indices[i] = p[i];
        }
        std::cout << "Loaded " << out.mesh.indices.size() << " indices." << std::endl;
    }

    // ---- 2. JOINTS_0 / WEIGHTS_0 読み取り ----
    {
        const auto& accJ = model.accessors[prim.attributes.at("JOINTS_0")];
        const auto& bvJ  = model.bufferViews[accJ.bufferView];
        const auto& bufJ = model.buffers[bvJ.buffer];
        
        // Calculate correct offset (bufferView offset + accessor offset)
        size_t offset = bvJ.byteOffset + accJ.byteOffset;
        const unsigned char* basePtr = &bufJ.data[offset];
        
        out.joints0.resize(accJ.count);
        for (int i = 0; i < accJ.count; i++) {
            out.joints0[i] = glm::ivec4(
                static_cast<int>(basePtr[i*4+0]),
                static_cast<int>(basePtr[i*4+1]),
                static_cast<int>(basePtr[i*4+2]),
                static_cast<int>(basePtr[i*4+3])
            );
        }
        
        std::cout << "Loaded " << out.joints0.size() << " joint indices." << std::endl;
    }
    {
        const auto& accW = model.accessors[prim.attributes.at("WEIGHTS_0")];
        const auto& bvW  = model.bufferViews[accW.bufferView];
        const auto& bufW = model.buffers[bvW.buffer];
        
        // Calculate correct offset (bufferView offset + accessor offset)
        size_t offset = bvW.byteOffset + accW.byteOffset;
        const float* p = reinterpret_cast<const float*>(&bufW.data[offset]);

        out.weights0.resize(accW.count);

        for (int i = 0; i < accW.count; i++) {
            out.weights0[i] = glm::vec4(p[i*4+0], p[i*4+1], p[i*4+2], p[i*4+3]);
        }
        std::cout << "Loaded " << out.weights0.size() << " joint weights." << std::endl;
    }

    // ---- 3. Skin (bone) の構築 ----
    const tinygltf::Skin& skin = model.skins[0];

    // // inverseBindMatrices
    {
        const auto& acc = model.accessors[skin.inverseBindMatrices];
        const auto& bv  = model.bufferViews[acc.bufferView];
        const auto& buf = model.buffers[bv.buffer];
        const float* p  = reinterpret_cast<const float*>(&buf.data[bv.byteOffset]);

        out.joints.resize(skin.joints.size());

        for (int i = 0; i < skin.joints.size(); i++) {
            out.joints[i].invBind = glm::make_mat4(p + i*16);
        }

        std::cout << "Loaded " << out.joints.size() << " inverse bind matrices." << std::endl;
    }

    // Create a map from node index to joint index
    std::map<int, int> nodeToJoint;
    for (int i = 0; i < skin.joints.size(); i++) {
        nodeToJoint[skin.joints[i]] = i;
    }

    // joints: local transform + parent
    for (int i = 0; i < skin.joints.size(); i++) {
        int nodeIdx = skin.joints[i];
        const tinygltf::Node& node = model.nodes[nodeIdx];

        // Find parent node index
        int parentNodeIdx = -1;
        for (int j = 0; j < model.nodes.size(); j++) {
            const tinygltf::Node& potentialParent = model.nodes[j];
            for (int c : potentialParent.children) {
                if (c == nodeIdx) {
                    parentNodeIdx = j;
                    break;
                }
            }
            if (parentNodeIdx != -1) break;
        }
        
        // Convert parent node index to joint index
        if (parentNodeIdx != -1 && nodeToJoint.find(parentNodeIdx) != nodeToJoint.end()) {
            out.joints[i].parent = nodeToJoint[parentNodeIdx];
        } else {
            out.joints[i].parent = -1;
        }

        // local transform
        glm::vec3 translation(0.0f);
        glm::vec4 rotation(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec3 scale(1.0f);

        if (node.translation.size() == 3) {
            translation = glm::vec3(
                static_cast<float>(node.translation[0]),
                static_cast<float>(node.translation[1]),
                static_cast<float>(node.translation[2])
            );
        }
        if (node.rotation.size() == 4) {
            rotation = glm::vec4(
                static_cast<float>(node.rotation[0]),
                static_cast<float>(node.rotation[1]),
                static_cast<float>(node.rotation[2]),
                static_cast<float>(node.rotation[3])
            );
        }
        if (node.scale.size() == 3) {
            scale = glm::vec3(
                static_cast<float>(node.scale[0]),
                static_cast<float>(node.scale[1]),
                static_cast<float>(node.scale[2])
            );
        }

        out.joints[i].local =
            glm::translate(glm::mat4(1.0f), translation) *
            glm::mat4_cast(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z)) *
            glm::scale(glm::mat4(1.0f), scale);
        // std::cout << "joint number " << i << " parent: " << out.joints[i].parent << std::endl;
    }

    out.mesh.upload();
    return out;
}