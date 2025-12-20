#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "scene.hpp"
class UI{
public:
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
    void drawRenderObjectTree(RenderObject* obj) {
        if (ImGui::TreeNode(obj->name.c_str())){
            ImGui::Text("Transform");
            ImGui::DragFloat3("Position", &obj->transform.position.x, 0.1f);
            ImGui::DragFloat4("Rotation", &obj->transform.rotation.x, 0.1f);
            ImGui::DragFloat3("Scale", &obj->transform.scale.x, 0.1f, 0.01f, 10.0f);

            // draw child
            if (obj->child) {
                drawRenderObjectTree(obj->child);
            }

            ImGui::TreePop();
        }
    };

    void drawInspector(Scene& scene){
        ImGui_ImplOpenGL3_NewFrame(); // imgui new frame
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Inspector");
        // add inspector elements here

        // edit transform
        

        
        for (auto& obj : scene.renderObjects){
            drawRenderObjectTree(obj);
        }
        // for (auto& obj : scene.renderObjects){
        //     if (ImGui::TreeNode(obj->name.c_str())){
        //         ImGui::Text("Transform");
        //         ImGui::DragFloat3("Position", &obj->transform.position.x, 0.1f);
        //         ImGui::DragFloat4("Rotation", &obj->transform.rotation.x, 0.1f);
        //         ImGui::DragFloat3("Scale", &obj->transform.scale.x, 0.1f, 0.01f, 10.0f);
        //         ImGui::TreePop();
        //     }
        // }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
};