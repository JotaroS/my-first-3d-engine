#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "renderobject.hpp"
#include "scene.hpp"
class UI{
public:
    RenderObject* selectedObj = nullptr;
    
    void draw()
    {
        ImGui_ImplOpenGL3_NewFrame(); // imgui new frame
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Info");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        // color picker
        static float bgColor[3] = {0.1f, 0.1f, 0.12f};
        ImGui::ColorPicker3("backgroundColor", bgColor);

        // header
        ImGui::Separator();


        ImGui::End();
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    // recursively list all render objects
    void drawRenderObjectTree(RenderObject* obj, Scene& scene) {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        
        // Add Selected flag if this object is selected
        if (obj->isSelected) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }
        
        bool nodeOpen = ImGui::TreeNodeEx(obj->name.c_str(), flags);
        
        // Check if the node was clicked to select it
        if (ImGui::IsItemClicked()) {
            // Deselect all objects first
            for (auto& otherObj : scene.renderObjects) {
                otherObj->isSelected = false;
            }
            // Select this one
            obj->isSelected = true;
            selectedObj = obj;
        }

        if (nodeOpen) {
            // draw child
            if (obj->child) {
                drawRenderObjectTree(obj->child, scene);
            }
            ImGui::TreePop();
        }
    }

    void drawInspector(RenderObject* obj){
        if (!obj) return;

        ImGui::Begin("Inspector");

        ImGui::Text("Selected Object: %s", obj->name.c_str());

        // Transform info
        ImGui::Separator();
        ImGui::Text("Transform");
        ImGui::InputFloat3("Position", &obj->transform.position.x);
        ImGui::InputFloat4("Rotation (quat)", &obj->transform.rotation.x);
        ImGui::InputFloat3("Scale", &obj->transform.scale.x);

        ImGui::End();
    }

    void drawHierarchy(Scene& scene) {
        ImGui_ImplOpenGL3_NewFrame(); // imgui new frame
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Hierarchy");

        for (auto& obj : scene.renderObjects){
            drawRenderObjectTree(obj, scene);
        }

        ImGui::End();
        
        // Draw inspector for selected object (outside hierarchy)
        if (selectedObj) {
            drawInspector(selectedObj);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());;

    }
};