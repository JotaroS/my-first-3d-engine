// This file provides the implementation of tinygltf
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "gltfloader.hpp"
#include <iostream>


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
    }

    // ---- 2. JOINTS_0 / WEIGHTS_0 読み取り ----
    // {
    //     auto& accJ = model.accessors[prim.attributes.at("JOINTS_0")];
    //     auto& bvJ  = model.bufferViews[accJ.bufferView];
    //     auto& bufJ = model.buffers[bvJ.buffer];
    //     const uint16_t* p = reinterpret_cast<const uint16_t*>(&bufJ.data[bvJ.byteOffset]);

    //     out.joints0.resize(accJ.count);

    //     for (int i = 0; i < accJ.count; i++) {
    //         out.joints0[i] = glm::ivec4(p[i*4+0], p[i*4+1], p[i*4+2], p[i*4+3]);
    //     }
    // }
    // {
    //     auto& accW = model.accessors[prim.attributes.at("WEIGHTS_0")];
    //     auto& bvW  = model.bufferViews[accW.bufferView];
    //     auto& bufW = model.buffers[bvW.buffer];
    //     const float* p = reinterpret_cast<const float*>(&bufW.data[bvW.byteOffset]);

    //     out.weights0.resize(accW.count);

    //     for (int i = 0; i < accW.count; i++) {
    //         out.weights0[i] = glm::vec4(p[i*4+0], p[i*4+1], p[i*4+2], p[i*4+3]);
    //     }
    // }

    // ---- 3. Skin (bone) の構築 ----
    // const tinygltf::Skin& skin = model.skins[0];

    // // inverseBindMatrices
    // {
    //     const auto& acc = model.accessors[skin.inverseBindMatrices];
    //     const auto& bv  = model.bufferViews[acc.bufferView];
    //     const auto& buf = model.buffers[bv.buffer];
    //     const float* p  = reinterpret_cast<const float*>(&buf.data[bv.byteOffset]);

    //     out.joints.resize(skin.joints.size());

    //     for (int i = 0; i < skin.joints.size(); i++) {
    //         out.joints[i].invBind = glm::make_mat4(p + i*16);
    //     }
    // }

    // joints: local transform + parent
    // for (int i = 0; i < skin.joints.size(); i++) {
    //     int nodeId = skin.joints[i];
    //     const auto& node = model.nodes[nodeId];

    //     // local matrix
    //     if (node.matrix.size() == 16) {
    //         out.joints[i].local = glm::make_mat4(node.matrix.data());
    //     } else {
    //         glm::mat4 T = glm::translate(glm::mat4(1), glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
    //         glm::quat q = glm::quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
    //         glm::mat4 R = glm::mat4_cast(q);
    //         glm::mat4 S = glm::scale(glm::mat4(1), glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
    //         out.joints[i].local = T * R * S;
        // }
        // parent index
        // out.joints[i].parent = node.parent; // tinygltf では自前で設定すると良い

        // std::vector<int> parentIndices(model.nodes.size(), -1);
        // for (int j = 0; j < model.nodes.size(); j++) {
        //     const auto& n = model.nodes[j];
        //     for (const int childId : n.children) {
        //         parentIndices[childId] = j;
        //     }
        // }
        // out.joints[i].parent = parentIndices[nodeId];
    // }

    out.mesh.upload();
    return out;
}