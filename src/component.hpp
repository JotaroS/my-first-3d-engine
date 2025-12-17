#pragma once

// Forward declarations to avoid circular dependency
class RenderObject;
class Renderer;

class Component
{
public:
    RenderObject* renderObject = nullptr;
    virtual ~Component() {}
    virtual void update() = 0;
    virtual void draw(Renderer& renderer) = 0;
};