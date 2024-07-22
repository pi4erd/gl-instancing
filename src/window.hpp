#pragma once

#include <GLFW/glfw3.h>

#include <string>

class Window {
public:
    Window(std::string title);
    ~Window();

    bool shouldClose(void) { return glfwWindowShouldClose(window); }
    void close(void) { glfwSetWindowShouldClose(window, true); }
    void pollEvents(void) { glfwPollEvents(); }
    void swapBuffers(void) { glfwSwapBuffers(window); }

    void toggleFullscreen() {
        if(!fullscreen) {
            GLFWmonitor *monitor = glfwGetPrimaryMonitor();
            int width, height, xpos, ypos;
            glfwGetMonitorWorkarea(monitor, &xpos, &ypos, &width, &height);

            glfwSetWindowMonitor(window, monitor, xpos, ypos, width, height, GLFW_DONT_CARE);
            fullscreen = true;
        } else {
            glfwSetWindowMonitor(window, nullptr, 0, 0, 1280, 720, GLFW_DONT_CARE);
            fullscreen = false;
        }
    }

    int width, height;

protected: // Virtuals
    virtual void mouseScroll(double dx, double dy) {
        (void)dx, (void)dy;
    }
    virtual void resize(int width, int height) {
        (void)width, (void)height;
    }
    virtual void keyboardCallback(int key, int action, int scancode, int mod) {
        (void)key;
        (void)action;
        (void)scancode;
        (void)mod;
    }
    virtual void mouseButton(int key, int action, int mod) {
        (void)key;
        (void)action;
        (void)mod;
    }
    GLFWwindow *getWindow(void) { return window; }

private:
    bool fullscreen = false;

    GLFWwindow *window;
};
