#pragma once
#include "component.hpp"
#include "mesh.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Joint {
    int parent = -1;
    glm::mat4 local;      // local bind transform
    glm::mat4 invBind;    // inverse bind matrix
    glm::mat4 world;      // computed each frame
};

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
        const std:: vector<Joint>& jnts,
        const std::vector<glm::ivec4>& jointIndices,
        const std::vector<glm::vec4>& jointWeights
    ) {
        originalMesh = mesh;
        deformedMesh = mesh; // start with a copy
        joints = jnts;
        joints0 = jointIndices;
        weights0 = jointWeights;

        deformedMesh.upload(); // upload deformed mesh to GPU
    }

    void update() override {
        fk();
        cpuSkin();
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

        for (int i = 0; i < pos.size(); i++) {
            glm::vec4 P = glm::vec4(pos[i], 1.0);
            glm::vec3 dst(0);

            glm::ivec4 jId = joints0[i];
            glm::vec4  w   = weights0[i];

            for (int k = 0; k < 4; k++) {
                int j = jId[k];
                float wk = w[k];
                if (wk <= 0.0f) continue;

                glm::mat4 M = joints[j].world * joints[j].invBind;
                dst += glm::vec3(M * P) * wk;
            }

            out[i] = dst;
        }

        // GPU に転送（頂点だけ差し替え）
        deformedMesh.updatePositionsOnly();
    }
};