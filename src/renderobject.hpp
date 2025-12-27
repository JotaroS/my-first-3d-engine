#pragma once
#include <vector>
#include <string>
#include "transform.hpp"
#include "component.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Forward declaration
class Renderer;

using namespace std;

class RenderObject {
public:
    string name;
    Transform transform; // local transform
    RenderObject* parent = nullptr;
    RenderObject* child = nullptr;
    vector<Component*> components;
    bool isSelected = false;
    RenderObject(const string& n) : name(n) {

    }
    ~RenderObject() {
        for (auto comp : components) {
            delete comp;
        }
    }

    template <typename T, typename... Args>
    T& addComponent(Args&&... args) {
        T* comp = new T(std::forward<Args>(args)...);
        comp->renderObject = this;  // Set the back-reference
        components.push_back(comp);
        return *comp;
    }

    // getComponent<T>()
    template <typename T>
    T* getComponent() {
        for (auto comp : components) {
            T* casted = dynamic_cast<T*>(comp);
            if (casted) {
                return casted;
            }
        }
        return nullptr;
    }

    void draw(Renderer& renderer) {

        // if there is parent then set transform parent
        if (parent) {
            transform.parent = &parent->transform;
        } else {
            transform.parent = nullptr;
        }
        for (auto& comp : components) {
            comp->draw(renderer);
        }

        // draw child
        if (child) {
            child->draw(renderer);
        }
    }

    void addChild(RenderObject* childObj) {
        childObj->parent = this;
        this->child = childObj;
    }

    void drawInspector(){
        ImGui::Begin("Inspector");

        ImGui::Text("Selected Object: %s", name.c_str());

        // Transform info
        ImGui::Separator();
        ImGui::Text("Transform");
        ImGui::DragFloat3("Position", &transform.position.x, 0.1f);
        ImGui::DragFloat4("Rotation (quat)", &transform.rotation.x, 0.01f);
        ImGui::DragFloat3("Scale", &transform.scale.x, 0.01f, 0.01f, 10.0f);
        ImGui::Separator();
        // draw components inspector
        for (auto& comp : components) {
            comp->drawInspector();
            // space
            ImGui::Separator();
        }

        ImGui::End();
        return;
    }
};