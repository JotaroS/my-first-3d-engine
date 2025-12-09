// let's goooooooo
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "primitives.hpp"
#include "renderer.hpp"
#include "renderobject.hpp"
#include "meshrenderer.hpp"
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

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return -1;
    }

    
    RenderObject cubeObj("Cube");
    auto& cubeRenderer = cubeObj.addComponent<MeshRenderer>();
    cubeRenderer.setMesh(Primitives::makeSphere(0.3f));

    Renderer renderer;
    renderer.init();

    GltfSkinData skinData;
    try {
        std::string modelPath = "../models/lion.gltf";
        std::cout << "Loading GLTF file: " << modelPath << std::endl;
        skinData = loadGltfSkin(modelPath);
        std::cout << "GLTF file loaded successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load GLTF: " << e.what() << std::endl;
        // Continue without the model
    }
    RenderObject lionObj("Lion");
    auto& lionRenderer = lionObj.addComponent<MeshRenderer>();
    lionRenderer.setMesh(skinData.mesh);

    while (!glfwWindowShouldClose(window))
    {
        renderer.draw();
        // slightly rotate the cube around y axis (quaternion
        cubeObj.transform.rotation = glm::rotate(cubeObj.transform.rotation, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        cubeObj.draw(renderer);
        lionObj.draw(renderer);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}