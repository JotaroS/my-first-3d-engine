// let's goooooooo
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "primitives.hpp"
#include "renderer.hpp"
#include "renderobject.hpp"
#include "meshrenderer.hpp"
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
    cubeRenderer.setMesh(Primitives::makeCube(1.0f));

    Renderer renderer;
    renderer.init();

    while (!glfwWindowShouldClose(window))
    {
        renderer.draw();
        cubeObj.draw(renderer);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}