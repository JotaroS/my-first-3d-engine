#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "mesh.hpp"
class Renderer {
public: 
    void init();
    void draw();
    void drawMesh(Mesh& m, glm::mat4 model, glm::vec3 color);
    // mouse, keyboard etc

private: 
    unsigned int shader = 0;
    glm::mat4 proj, view, model;

    int width = 800;
    int height = 600;

    float cameraDist = 5.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;

    void updateCamera();
};