#pragma once
#include <vector>
#include <string>
#include "transform.hpp"
#include "component.hpp"

// Forward declaration
class Renderer;

using namespace std;

class RenderObject {
public:
    string name;
    Transform transform;
    vector<Component*> components;
    RenderObject(const string& n) : name(n) {
        
    }
    ~RenderObject() {
        for (auto comp : components) {
            delete comp;
        }
    }

    template <typename T, typename... Args>
    T& addComponent(Args&&... args) {
        T* comp = new T(std::forward<Args>(args)...);
        comp->renderObject = this;  // Set the back-reference
        components.push_back(comp);
        return *comp;
    }

    // getComponent<T>()
    template <typename T>
    T* getComponent() {
        for (auto comp : components) {
            T* casted = dynamic_cast<T*>(comp);
            if (casted) {
                return casted;
            }
        }
        return nullptr;
    }

    void draw(Renderer& renderer) {
        for (auto& comp : components) {
            comp->draw(renderer);
        }
    }
};