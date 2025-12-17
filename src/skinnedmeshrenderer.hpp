#pragma once
#include "component.hpp"
#include "mesh.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "joint.hpp"
#include "renderobject.hpp"
#include "renderer.hpp"
#include <iostream>
using namespace std;
class SkinnedMeshRenderer : public Component {
public:
    Mesh originalMesh;
    Mesh deformedMesh;
    std::vector<Joint> joints;

    // vertexAttributes
    std::vector<glm::ivec4> joints0; // i vec4 means 4 joint indices per vertex
    std::vector<glm::vec4> weights0;

    void setGLTFMesh
    (
        const Mesh& mesh,
        const std:: vector<Joint>& jnts, // joints from skin
        const std::vector<glm::ivec4>& jointIndices, // JOINTS_0
        const std::vector<glm::vec4>& jointWeights // WEIGHTS_0
    ) {
        originalMesh = mesh;
        deformedMesh = mesh; // start with a copy
        joints = jnts;
        joints0 = jointIndices;
        weights0 = jointWeights;

        // Debug: Check array sizes
        std::cout << "Mesh positions: " << mesh.positions.size() << std::endl;
        std::cout << "Joint indices: " << jointIndices.size() << std::endl;
        std::cout << "Weights: " << jointWeights.size() << std::endl;
        std::cout << "Joints: " << jnts.size() << std::endl;

        // Initialize with rest pose
        update();
        
        deformedMesh.upload(); // upload deformed mesh to GPU
    }

    void update() override {
        fk();
        cpuSkin();
    }

    void draw(Renderer& renderer) override {
        renderer.drawMesh(deformedMesh, renderObject->transform.worldMatrix(), glm::vec3(1.0f, 1.0f, 1.0f));
    }

    void fk(){
        for(size_t i=0; i<joints.size(); i++){
            if(joints[i].parent == -1){
                joints[i].world = joints[i].local;
            } else {
                joints[i].world = joints[joints[i].parent].world * joints[i].local;
            }
        }
    }

    // --- CPU スキニング ---
    void cpuSkin() {
        auto& pos = originalMesh.positions;
        auto& out = deformedMesh.positions;

        // Safety check: ensure arrays match
        if (pos.size() != joints0.size() || pos.size() != weights0.size()) {
            std::cerr << "ERROR: Array size mismatch! pos=" << pos.size() 
                      << " joints0=" << joints0.size() 
                      << " weights0=" << weights0.size() << std::endl;
            return;
        }

        static bool firstRun = true;
        
        for (size_t i = 0; i < pos.size(); i++) {
            glm::vec4 P = glm::vec4(pos[i], 1.0);
            glm::vec3 dst(0);

            glm::ivec4 jId = joints0[i];
            glm::vec4  w   = weights0[i];

            
            for (int k = 0; k < 4; k++) {
                int j = jId[k];
                float wk = w[k];
                if (wk <= 0.0f) continue;
                
                // Validate joint index
                if (j < 0 || j >= static_cast<int>(joints.size())) {
                    std::cerr << "Invalid joint index: " << j << " at vertex " << i << std::endl;
                    continue;
                }
                glm::mat4 M = joints[j].world * joints[j].invBind;
                dst += glm::vec3(M * P) * wk;
            }
            
            out[i] = dst;
            
            
        }

        // GPU に転送（頂点だけ差し替え）
        deformedMesh.updatePositionsOnly();
    }
};