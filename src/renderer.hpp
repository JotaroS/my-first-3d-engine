#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "mesh.hpp"
#include "scene.hpp"
class Renderer {
public: 
    Scene scene;
    void init();
    void init(int windowWidth, int windowHeight);
    void draw();
    void drawMesh(Mesh& m, glm::mat4 model, glm::vec3 color);
    void drawScene(Scene& scene);
    void drawGridPlane(float size = 10.0f, int divisions = 10);
    void handleInput(float dt);
    void drawImGui();
    // mouse, keyboard etc

private: 
    unsigned int shader = 0;
    glm::mat4 proj, view, model;

    int width = 800;
    int height = 600;

    float cameraDist = 5.0f;
    float yaw = -90.0f;
    float pitch = 0.0f;

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, cameraDist);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

    void updateCamera();
    
    
};