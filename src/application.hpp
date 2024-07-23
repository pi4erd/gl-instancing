#pragma once

#include "camera.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "window.hpp"
#include "imgui.hpp"

#include <memory>
#include <mutex>
#include <condition_variable>

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
    void updateThread();

    void updateDesync(double deltaTime);
    void update(double deltaTime);
    void render(double deltaTime);

    void render_ui(double deltaTime);

private: // helpers
    static std::unique_ptr<std::vector<glm::vec3>> generateRandomVectors(size_t size, float min, float max);

private: // stack allocated (default constructor)
    ImguiInstance imguiInstance;

    // Camera
    Camera camera;
    glm::vec3 cameraRotation;
    float controlSpeed = 10.0;
    double prevMouseX, prevMouseY;
    
    // Simulation
    float timePassed = 0.0f;
    float lastUpdateTickTime = 0.0f;
    float timeScale = 0.01f;
    bool simRunning = true;

    // Buffers
    GLuint instanceVBO;

    // Threading
    std::mutex simMutex;
    std::condition_variable simCondition;

    // Additional
    bool wireframeOn = false;

private: // smart ptrs / heap
    std::shared_ptr<Mesh> cubeMesh;
    std::shared_ptr<Material> mat;

    std::unique_ptr<std::vector<glm::vec3>> cubePositions;
    std::unique_ptr<std::vector<glm::vec3>> cubeVelocities;
};
