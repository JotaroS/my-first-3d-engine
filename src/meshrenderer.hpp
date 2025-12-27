#pragma once
#include "component.hpp"
#include "mesh.hpp"
#include "renderer.hpp"
#include "renderobject.hpp"
#include <iostream>
class MeshRenderer : public Component {
public:
    Mesh mesh;
    glm:: vec3 color {1.0f, 1.0f, 1.0f};

    void setMesh(const Mesh& m) {
        mesh = m;
    }

    void draw(Renderer& renderer) override {
        if (renderObject == nullptr)
        {
            std::cerr << "Error: renderObject is nullptr in MeshRenderer::draw\n";
            return;
        }

        renderer.drawMesh(mesh,
            renderObject->transform.worldMatrix(),
            color);
    }
    void update() override {
        // No dynamic updates needed for now
    }

    void drawInspector() override {
        ImGui::Text("Mesh Renderer Component");
        ImGui::ColorEdit3("Mesh Color", &color[0]);
    }
};