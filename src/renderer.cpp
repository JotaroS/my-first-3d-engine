#include "renderer.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
// Utility: shader compile
static unsigned int compileShader(const char* vsPath, const char* fsPath) {
    auto load = [](const char* p) {
        std::ifstream f(p);
        if (!f.is_open()) {
            std::cerr << "Failed to open file: " << p << std::endl;
            return std::string();
        }
        std::stringstream ss;
        ss << f.rdbuf();
        return ss.str();
    };

    std::string vsSrc = load("../shaders/basic.vert");
    std::string fsSrc = load("../shaders/basic.frag");

    auto compile = [](GLenum type, const std::string& src) {
        GLuint s = glCreateShader(type);
        const char* c = src.c_str();
        glShaderSource(s, 1, &c, nullptr);
        glCompileShader(s);
        return s;
    };

    GLuint vs = compile(GL_VERTEX_SHADER, vsSrc);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fsSrc);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return prog;
}

void Renderer::init(){
    shader = compileShader("../shaders/basic.vert", "../shaders/basic.frag");

    glEnable(GL_DEPTH_TEST); // Enable depth testing
    glEnable(GL_CULL_FACE);  // Enable face culling
    // glCullFace(GL_BACK);     // Cull back faces

    proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    updateCamera();
}

void Renderer::init(int windowWidth, int windowHeight){
    width = windowWidth;
    height = windowHeight;
    init();
}

void Renderer::updateCamera(){
    // Calculate forward direction from yaw and pitch
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
    
    // Use lookAt for FPS camera
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Renderer::handleInput(float dt){
    float vel = 5.0f * dt;
    
    // Calculate right vector for strafing
    glm::vec3 right = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
    
    // W/S - move forward/backward in camera direction (XZ plane only for FPS)
    glm::vec3 frontXZ = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += frontXZ * vel;
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= frontXZ * vel;
    }
    // A/D - strafe left/right
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= right * vel;
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += right * vel;
    }
    // Q/E - move up/down
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_Q) == GLFW_PRESS) {
        cameraPos.y -= vel;
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_E) == GLFW_PRESS) {
        cameraPos.y += vel;
    }

    // Mouse drag to rotate camera
    static double lastX = 0.0, lastY = 0.0;
    static bool firstClick = true;
    double xpos, ypos;
    glfwGetCursorPos(glfwGetCurrentContext(), &xpos, &ypos);
    
    if (glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (firstClick) {
            lastX = xpos;
            lastY = ypos;
            firstClick = false;
        }
        
        float xoffset = static_cast<float>(xpos - lastX);
        float yoffset = static_cast<float>(lastY - ypos); // Reversed: y goes up

        float sensitivity = 0.2f;
        yaw += xoffset * sensitivity;
        pitch += yoffset * sensitivity;

        // Constrain pitch
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
        
        lastX = xpos;
        lastY = ypos;
    } else {
        firstClick = true;
    }

    updateCamera();
}

void Renderer::drawGridPlane(float size, int divisions){
    static unsigned int gridVAO = 0;
    static unsigned int gridVBO = 0;
    static unsigned int gridNBO = 0;
    
    // Initialize grid VAO/VBO on first call
    if (gridVAO == 0) {
        glGenVertexArrays(1, &gridVAO);
        glGenBuffers(1, &gridVBO);
        glGenBuffers(1, &gridNBO);
    }
    
    // Build grid line vertices and normals
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    float halfSize = size / 2.0f;
    
    for (int i = 0; i <= divisions; ++i) {
        float coord = -halfSize + i * (size / divisions);
        
        // Lines along X axis (parallel to X)
        vertices.push_back(glm::vec3(-halfSize, 0.0f, coord));
        vertices.push_back(glm::vec3(halfSize, 0.0f, coord));
        normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        
        // Lines along Z axis (parallel to Z)
        vertices.push_back(glm::vec3(coord, 0.0f, -halfSize));
        vertices.push_back(glm::vec3(coord, 0.0f, halfSize));
        normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
    // Upload to GPU
    glBindVertexArray(gridVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, gridNBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(1);
    
    // Draw the grid
    glUseProgram(shader);
    
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "uProj"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3f(glGetUniformLocation(shader, "uColor"), 0.6, 0.6, 0.6);
    
    glDrawArrays(GL_LINES, 0, vertices.size());
    
    glBindVertexArray(0);
}

void Renderer::drawScene(Scene& scene){
    
    // draw background and set lights
    glClearColor(scene.backgroundColor.r, scene.backgroundColor.g, scene.backgroundColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glUseProgram(shader);

    // matrices
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uView"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uProj"), 1, GL_FALSE, &proj[0][0]);

    // set lights from scene
    for (size_t i = 0; i < scene.lights.size(); i++) {
        std::string posName = "lightPos[" + std::to_string(i) + "]";
        std::string colorName = "lightColor[" + std::to_string(i) + "]";
        glUniform3f(glGetUniformLocation(shader, posName.c_str()), 
                    scene.lights[i].position.x, 
                    scene.lights[i].position.y, 
                    scene.lights[i].position.z);
        glUniform3f(glGetUniformLocation(shader, colorName.c_str()), 
                    scene.lights[i].color.x, 
                    scene.lights[i].color.y, 
                    scene.lights[i].color.z);
    }
    glUniform3f(glGetUniformLocation(shader, "ambientColor"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(shader, "uColor"), 0.8f, 0.8f, 0.85f);


    for(auto obj : scene.renderObjects){
       obj->draw(*this);
    }
}

void Renderer::draw() {
    glClearColor(0.12f, 0.12f, 0.14f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);

    // matrices
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uView"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uProj"), 1, GL_FALSE, &proj[0][0]);

    // ---- Three-point lighting ----
    glUniform3f(glGetUniformLocation(shader, "lightPos[0]"), 3, 4, 2);   // Key
    glUniform3f(glGetUniformLocation(shader, "lightPos[1]"), -2, 1, 3);  // Fill
    glUniform3f(glGetUniformLocation(shader, "lightPos[2]"), -3, 5, -4); // Rim

    glUniform3f(glGetUniformLocation(shader, "lightColor[0]"), 1.0, 0.95, 0.9);
    glUniform3f(glGetUniformLocation(shader, "lightColor[1]"), 0.6, 0.7, 1.0);
    glUniform3f(glGetUniformLocation(shader, "lightColor[2]"), 1.0, 0.7, 0.4);

    glUniform3f(glGetUniformLocation(shader, "ambientColor"), 0.2f, 0.2f, 0.2f);

    glUniform3f(glGetUniformLocation(shader, "uColor"), 0.8f, 0.8f, 0.85f);
}



void Renderer::drawMesh(Mesh& m, glm::mat4 model, glm::vec3 color){
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"), 1, GL_FALSE, &model[0][0]);
    glUniform3f(glGetUniformLocation(shader, "uColor"), color.r, color.g, color.b);
    m.draw();
}


void Renderer::drawImGui(){
    // draw fps, camera pos, camera rot and etc
    ImGui::Begin("Renderer Info");
    ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", cameraPos.x, cameraPos.y, cameraPos.z);
    ImGui::Text("Camera Front: (%.2f, %.2f, %.2f)", cameraFront.x, cameraFront.y, cameraFront.z);
    ImGui::Text("Yaw: %.2f, Pitch: %.2f", yaw, pitch);
    ImGui::Text("Camera Distance: %.2f", cameraDist);
    ImGui::End();
}