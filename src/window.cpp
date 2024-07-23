#include <GL/glew.h>
#include "window.hpp"

#include <stdexcept>

#include "log.hpp"

Window::Window(std::string title)
{
    if(!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW!");

    LOG_DEBUG("Initialized GLFW.");

    glfwWindowHint(GLFW_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);
    // glfwWindowHint(GLFW_WAYLAND_APP_ID, 133753535);

    width = 1280;
    height = 720;

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if(!window)
        throw std::runtime_error("Failed to create window! Maybe OpenGL 4.6 isn't supported?");

    LOG_DEBUG("Created window.");

    glfwSetWindowUserPointer(window, this);
    auto resize = [](GLFWwindow *window, int width, int height) {
        static_cast<Window*>(glfwGetWindowUserPointer(window))->resize(width, height);
    };
    glfwSetWindowSizeCallback(window, resize);
    auto mouseWheel = [](GLFWwindow *window, double dx, double dy) {
        static_cast<Window*>(glfwGetWindowUserPointer(window))->mouseScroll(dx, dy);
    };
    glfwSetScrollCallback(window, mouseWheel);
    auto keyboardCall = [](GLFWwindow *window, int key, int scancode, int action, int mod) {
        static_cast<Window*>(glfwGetWindowUserPointer(window))->keyboardCallback(key, action, scancode, mod);
    };
    glfwSetKeyCallback(window, keyboardCall);

    glfwMakeContextCurrent(window);

    glewExperimental = true;
    GLenum glewStatus = glewInit();
    if(glewStatus != GLEW_OK) {
        std::string error_str = (const char*)glewGetErrorString(glewStatus);

        throw std::runtime_error(fmt::format("Failed to initialize glew: {}", error_str));
    }

    glfwSwapInterval(1);
}

Window::~Window()
{
    LOG_DEBUG("Stopping GLFW.");

    glfwDestroyWindow(window);
    glfwTerminate();
}
