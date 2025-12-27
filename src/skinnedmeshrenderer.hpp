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
        // Debug: Check transform
        static int frameCount = 0;
        if (frameCount++ % 60 == 0) {
            std::cout << "Scale: " << renderObject->transform.scale.x << ", " 
                      << renderObject->transform.scale.y << ", " 
                      << renderObject->transform.scale.z << std::endl;
        }
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

    void drawInspector() override {
        ImGui::Text("Skinned Mesh Renderer Component");
        ImGui::Text("Number of Joints: %d", static_cast<int>(joints.size()));

        for (size_t i = 0; i < joints.size(); i++) {
            if (ImGui::TreeNode((void*)(intptr_t)i, "Joint %d", static_cast<int>(i))) {
                ImGui::Text("Parent: %d", joints[i].parent);
                ImGui::Text("Local Transform:");
                ImGui::Text("  [%.2f, %.2f, %.2f, %.2f]", joints[i].local[0][0], joints[i].local[0][1], joints[i].local[0][2], joints[i].local[0][3]);
                ImGui::Text("  [%.2f, %.2f, %.2f, %.2f]", joints[i].local[1][0], joints[i].local[1][1], joints[i].local[1][2], joints[i].local[1][3]);
                ImGui::Text("  [%.2f, %.2f, %.2f, %.2f]", joints[i].local[2][0], joints[i].local[2][1], joints[i].local[2][2], joints[i].local[2][3]);
                ImGui::Text("  [%.2f, %.2f, %.2f, %.2f]", joints[i].local[3][0], joints[i].local[3][1], joints[i].local[3][2], joints[i].local[3][3]);
                ImGui::TreePop();
            }
        }
    }
};