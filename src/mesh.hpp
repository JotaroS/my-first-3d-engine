
#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
using namespace std;
class Mesh
{
public:
    vector<glm::vec3> positions;
    vector<glm::vec3> normals;
    std::vector<unsigned int> indices;


    unsigned int vao = 0, vbo = 0, nbo = 0, ebo = 0; //

    void upload(); // Upload to GPU
    void draw() const; // Draw the mesh (constant function for safety) (why const? because it doesn't modify the mesh data)
    void updatePositionsOnly();
};