#pragma once

#include "camera.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "window.hpp"
#include "imgui.hpp"

#include <memory>

class Application : public Window {
public:
    Application();
    ~Application();

    void run();

protected:
    void resize(int width, int height) override;
    void mouseButton(int key, int action, int mod) override;
    void mouseScroll(double dx, double dy) override;
    void keyboardCallback(int key, int action, int scancode, int mod) override;

private: // methods
    void update(double deltaTime);
    void render(double deltaTime);

    void render_ui(double deltaTime);

private: // helpers
    static std::unique_ptr<std::vector<glm::vec3>> generateRandomPositions(size_t size, float min, float max);

private: // stack allocated (default constructor)
    ImguiInstance imguiInstance;

    Camera camera;
    glm::vec3 cameraRotation;
    float controlSpeed = 10.0;

    double prevMouseX, prevMouseY;

    GLuint instanceVBO;

private: // smart ptrs / heap
    std::shared_ptr<Mesh> testMesh;
    std::shared_ptr<Mesh> cubeMesh;
    std::shared_ptr<Material> mat;

    std::unique_ptr<std::vector<glm::vec3>> cubePositions;
};
