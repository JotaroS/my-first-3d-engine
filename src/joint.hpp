#include <glm/glm.hpp>
struct Joint {
    int parent = -1;
    glm::mat4 local;      // local bind transform
    glm::mat4 invBind;    // inverse bind matrix
    glm::mat4 world;      // computed each frame
};