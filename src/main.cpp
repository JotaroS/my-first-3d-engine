// let's goooooooo
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "primitives.hpp"
#include "meshrenderer.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "renderobject.hpp"
#include "skinnedmeshrenderer.hpp"
#include "gltfloader.hpp"
// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ui.hpp"

int main(void){
    if (!glfwInit())
        return -1;
    
    // Set OpenGL version to 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1600, 900, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    // Set V-Sync (1 = 60fps on most monitors, 0 = unlimited)
    glfwSwapInterval(1);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return -1;
    }

    // initialize imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    UI ui;


    Renderer renderer;
    renderer.init(1600, 900);

    Scene scene;
    
    // add key, fill, back lights
    scene.addLight(glm::vec3(3.0f, 4.0f, 2.0f));   // Key light
    scene.addLight(glm::vec3(-2.0f, 1.0f, 3.0f), glm::vec3(0.6f, 0.7f, 1.0f), 0.8f);  // Fill light
    scene.addLight(glm::vec3(3.0f, 5.0f, -4.0f), glm::vec3(1.0f, 0.7f, 0.4f), 0.6f); // Rim light
    scene.setBackground(glm::vec3(0.12f, 0.12f, 0.14f));
    

    renderer.scene = scene;

    GltfSkinData skinData;
    try {
        std::string modelPath = "../models/maybecaracter2rig2.gltf";
        std::cout << "Loading GLTF file: " << modelPath << std::endl;
        skinData = loadGltfSkin(modelPath);
        std::cout << "GLTF file loaded successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load GLTF: " << e.what() << std::endl;
        // Continue without the model
    }
    RenderObject lionObj("Humanoid");
    SkinnedMeshRenderer& skinnedComp = lionObj.addComponent<SkinnedMeshRenderer>();
    skinnedComp.setGLTFMesh(
        skinData.mesh,
        skinData.joints,
        skinData.joints0,
        skinData.weights0
    );
    lionObj.transform.scale = glm::vec3(0.1f);
    scene.renderObjects.push_back(&lionObj);
    float prev_time = static_cast<float>(glfwGetTime());

    RenderObject sphereObj("Sphere_1");
    RenderObject sphereObj2("Sphere_2");
    RenderObject sphereObj3("Sphere_3");

    sphereObj.addComponent<MeshRenderer>().setMesh(Primitives::makeSphere(0.1f, 36, 18));
    sphereObj2.addComponent<MeshRenderer>().setMesh(Primitives::makeSphere(0.1f, 36, 18));
    sphereObj3.addComponent<MeshRenderer>().setMesh(Primitives::makeSphere(0.1f, 36, 18));

    sphereObj.addChild(&sphereObj2);
    sphereObj2.addChild(&sphereObj3);

    scene.addRenderObject(&sphereObj);

    while (!glfwWindowShouldClose(window))
    {
        float now = static_cast<float>(glfwGetTime());
        float deltaTime = now - prev_time; // Simple delta time calculation
        prev_time = now;
        renderer.handleInput(deltaTime);
        renderer.drawScene(scene);
        renderer.drawGridPlane();
    
        // Rotate joint 10 like waving hands with sine wave
        // skinnedComp.joints[10].rotateAxisLocal(glm::vec3(0.0f, 0.0f, 1.0f), glm::sin(now * 3.0f) * glm::radians(0.5f));
        // skinnedComp.update();

        // Start the ImGui frame
        // ui.draw();
        ui.drawHierarchy(scene);

        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // save imgui settings
    ImGui::SaveIniSettingsToDisk("imgui.ini");

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}