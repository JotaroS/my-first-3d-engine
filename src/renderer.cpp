#include "renderer.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

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

void Renderer::updateCamera(){
    // Spherical to Cartesian conversion
    float x = cameraDist * cosf(pitch) * sinf(yaw);
    float y = cameraDist * sinf(pitch);
    float z = cameraDist * cosf(pitch) * cosf(yaw);

    glm::vec3 cameraPos = glm::vec3(x, y, z);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    view = glm::lookAt(cameraPos, target, up);
    proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f); 
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

    glUniform3f(glGetUniformLocation(shader, "uColor"), 0.8f, 0.8f, 0.85f);
}

void Renderer::drawMesh(Mesh& m, glm::mat4 model, glm::vec3 color){
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"), 1, GL_FALSE, &model[0][0]);
    glUniform3f(glGetUniformLocation(shader, "uColor"), color.r, color.g, color.b);
    m.draw();
}
