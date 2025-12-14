// let's goooooooo
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "primitives.hpp"
#include "renderer.hpp"
#include "renderobject.hpp"
#include "skinnedmeshrenderer.hpp"
#include "gltfloader.hpp"

int main(void){
    if (!glfwInit())
        return -1;
    
    // Set OpenGL version to 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
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

    

    Renderer renderer;
    renderer.init();

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
    RenderObject lionObj("Lion");
    SkinnedMeshRenderer& skinnedComp = lionObj.addComponent<SkinnedMeshRenderer>();
    skinnedComp.setGLTFMesh(
        skinData.mesh,
        skinData.joints,
        skinData.joints0,
        skinData.weights0
    );
    float prev_time = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(window))
    {
        float now = static_cast<float>(glfwGetTime());
        float deltaTime = now - prev_time; // Simple delta time calculation
        prev_time = now;
        // keyboard input
        renderer.handleInput(deltaTime);
        renderer.draw();
        // slightly rotate the cube around y axis (quaternion
        // lionObj.transform.rotation = glm::rotate(lionObj.transform.rotation, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // lionObj.transform.scale = glm::vec3(0.5f); // scale down the lion model
        skinnedComp.update();
        
        lionObj.draw(renderer);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}