#include "application.hpp"
#include "material.hpp"
#include "shader.hpp"
#include "log.hpp"

#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <thread>

#define RANDF(MIN, MAX) (static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (MAX - MIN))) + MIN)

Application::Application() : Window("My window"), imguiInstance(getWindow()), cameraRotation(0.0) {
    std::srand(time(nullptr));

    GLFWimage icons[1];
    icons[0].pixels = stbi_load("assets/appicon.png", &icons[0].width, &icons[0].height, nullptr, 4);
    glfwSetWindowIcon(getWindow(), 1, icons);
    stbi_image_free(icons[0].pixels);

    glViewport(0, 0, width, height);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    const GLubyte *version = glGetString(GL_VERSION);
    LOG_INFO("Version info: OpenGL {}", (const char*)version);

    glfwGetCursorPos(getWindow(), &prevMouseX, &prevMouseY);
    
    constexpr size_t cubeCount = 100000;

    cubePositions = generateRandomVectors(cubeCount, -1000.0, 1000.0);
    cubeVelocities = generateRandomVectors(cubeCount, -10.0, 10.0);

    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * cubePositions->size(), cubePositions->data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    cubeMesh = Mesh::createFromVertexArrayInstanced(
    { // vertices
        -1, -1, -1, -0.57735026919, -0.57735026919, -0.57735026919,
        1, -1, -1, 10.57735026919, -0.57735026919, -0.57735026919,
        1, 1, -1, 10.57735026919, 10.57735026919, -0.57735026919,
        -1, 1, -1, -0.57735026919, 10.57735026919, -0.57735026919,
        
        -1, -1, 1, -0.57735026919, -0.57735026919, 0.57735026919,
        1, -1, 1, 0.57735026919, -0.57735026919, 0.57735026919,
        1, 1, 1, 0.57735026919, 0.57735026919, 0.57735026919,
        -1, 1, 1, -0.57735026919, 0.57735026919, 0.57735026919,
    },
    { // indices
        0, 2, 1,
        0, 3, 2,
        
        5, 7, 4,
        5, 6, 7,

        0, 4, 3,
        4, 7, 3,

        1, 2, 5,
        5, 2, 6,

        2, 3, 6,
        3, 7, 6,

        0, 1, 4,
        1, 5, 4
    }, instanceVBO);

    auto vertShader = shaderFromGlslFile("shaders/cube.vert", GL_VERTEX_SHADER);
    auto fragShader = shaderFromGlslFile("shaders/cube.frag", GL_FRAGMENT_SHADER);
    
    mat = MaterialBuilder()
        .attachShader(vertShader)
        .attachShader(fragShader)
        .buildMaterial();

    camera.origin = glm::vec3(0.0, 0.0, -10.0);
    camera.direction = glm::vec3(0.0, 0.0, 1.0);
    
    // glClearColor(0.2, 0.2, 0.3, 1.0); // This is a pleasant color
    glClearColor(0.0, 0.0, 0.0, 1.0);
}

void Application::run()
{
    double prevTime = glfwGetTime();

    std::thread thread(&Application::updateThread, this);
    LOG_DEBUG("Dispatched update thread");

    while(!shouldClose()) {
        pollEvents();

        double time = glfwGetTime();
        double deltaTime = time - prevTime;
        prevTime = time;

        update(deltaTime);
        render(deltaTime);
    }

    simCondition.notify_all();
    thread.join();
    LOG_DEBUG("Joined update thread");
}

void Application::resize(int width, int height)
{
    glViewport(0, 0, width, height);
    this->width = width;
    this->height = height;
    LOG_DEBUG("Resized to {}x{}", width, height);
}

void Application::render(double deltaTime)
{
    if(simRunning) {
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * cubePositions->size(), cubePositions->data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat->use();
    mat->uniform4x4("projection_view", camera.projectionMatrix(width / (float)height) * camera.viewMatrix());
    cubeMesh->drawInstanced(cubePositions->size());
    
    render_ui(deltaTime);

    swapBuffers();
}

void Application::render_ui(double deltaTime)
{
    imguiInstance.newFrame();

    ImGui::Begin("Stats");
    ImGui::Text("Instance count: %lu", cubePositions->size());
    ImGui::Text("Frametime: %lfms", deltaTime * 1000.0);
    ImGui::Text("Last Update Tick Time: %fms", lastUpdateTickTime * 1000.0);
    ImGui::Text("Virtual time passed: %fs", timePassed);
    ImGui::DragFloat(
        "Time scale",
        &timeScale,
        0.0001, 0.0001, 2.0,
        "%.4f"
    );
    ImGui::End();

    ImGui::Begin("Camera");
    ImGui::DragFloat3(
        "Camera position",
        glm::value_ptr<float>(camera.origin),
        1.0f, -100000.0, 100000.0
    );
    ImGui::DragFloat3(
        "Camera rotation",
        glm::value_ptr<float>(cameraRotation),
        0.01f, -glm::two_pi<float>(), glm::two_pi<float>()
    );
    ImGui::DragFloat(
        "Camera speed",
        &controlSpeed,
        1.0f, 0.01f, 99999.0f, "%.3f",
        ImGuiSliderFlags_Logarithmic
    );
    ImGui::DragFloat(
        "Near Plane",
        &camera.nearPlane,
        0.01, 0.001, 1.0
    );
    ImGui::DragFloat(
        "Far Plane",
        &camera.farPlane,
        100.0, 100.0, 100000.0
    );
    ImGui::End();

    imguiInstance.renderFrame();
}

std::unique_ptr<std::vector<glm::vec3>> Application::generateRandomVectors(size_t size, float min, float max)
{
    std::unique_ptr<std::vector<glm::vec3>> positions = std::make_unique<std::vector<glm::vec3>>(size);

    for(glm::vec3 &position : *positions) {
        position = glm::vec3(
            RANDF(min, max),
            RANDF(min, max),
            RANDF(min, max)
        );
    }

    return std::move(positions);
}

void Application::updateThread()
{
    double prevTime = glfwGetTime();
    while(!shouldClose()) {
        double time = glfwGetTime();
        double deltaTime = time - prevTime;
        prevTime = time;

        if(deltaTime > 1.0) deltaTime = 0.0001;

        lastUpdateTickTime = deltaTime;
        updateDesync(deltaTime * timeScale);
    }
}

void Application::updateDesync(double deltaTime)
{
    std::unique_lock<std::mutex> lock(simMutex);
    simCondition.wait(lock);

    for(size_t i = 0; i < cubePositions->size(); i++) {
        glm::vec3 &position = (*cubePositions)[i];
        glm::vec3 &velocity = (*cubeVelocities)[i];
        glm::vec3 heading = 0.0f - position;
        glm::vec3 gravityVector = 1000000.0f * glm::normalize(heading) / ((float)heading.length() * heading.length());
        velocity += (
            glm::vec3(
                RANDF(-1.0f, 1.0f),
                RANDF(-1.0f, 1.0f),
                RANDF(-1.0f, 1.0f)
            ) * (float)deltaTime * 0.0f + gravityVector * (float)deltaTime
        );
        position += velocity * (float)deltaTime;
    }

    timePassed += deltaTime;
}

void Application::update(double deltaTime)
{
    if(glfwGetKey(getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
        camera.origin += camera.direction * (float)deltaTime * controlSpeed;
    }
    if(glfwGetKey(getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
        camera.origin -= camera.direction * (float)deltaTime * controlSpeed;
    }
    glm::vec3 up_agnostic = glm::normalize(glm::vec3(
        camera.direction.x,
        0.0f,
        camera.direction.z
    ));
    glm::vec3 right = glm::cross(up_agnostic, camera.up);
    if(glfwGetKey(getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
        camera.origin -= right * (float)deltaTime * controlSpeed;
    }
    if(glfwGetKey(getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
        camera.origin += right * (float)deltaTime * controlSpeed;
    }

    // LOG_DEBUG("camera.origin = {}, {}, {}", camera.origin.x, camera.origin.y, camera.origin.z);
    // LOG_DEBUG("camera.direction = {}, {}, {}", camera.direction.x, camera.direction.y, camera.direction.z);

    constexpr float sensitivity = 5;

    double mouseX, mouseY;
    glfwGetCursorPos(getWindow(), &mouseX, &mouseY);
    double dx = mouseX - prevMouseX, dy = mouseY - prevMouseY;
    prevMouseX = mouseX, prevMouseY = mouseY;

    if(glfwGetInputMode(getWindow(), GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
        cameraRotation.y += (float)dx / (float)width * sensitivity;
        cameraRotation.x += -(float)dy / (float)height * sensitivity;
    }
    
    constexpr float limitAngle = glm::half_pi<float>() - 0.01;

    cameraRotation.x = cameraRotation.x > limitAngle ? limitAngle :
        (cameraRotation.x < -limitAngle ? -limitAngle : cameraRotation.x);

    camera.direction = glm::vec4(0.0, 0.0, 1.0, 1.0) *
        glm::rotate(cameraRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    camera.direction = glm::vec4(camera.direction, 1.0) *
        glm::rotate(cameraRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));

    camera.direction = glm::vec4(camera.direction, 1.0) *
        glm::rotate(cameraRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));

    if(glfwGetMouseButton(getWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        glfwSetInputMode(getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetInputMode(getWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    } else {
        glfwSetInputMode(getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetInputMode(getWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
    }

    if(simRunning)
        simCondition.notify_all();
}

void Application::mouseButton(int key, int action, int mod)
{
}

void Application::keyboardCallback(int key, int action, int scancode, int mod)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        close();
    }

    if(key == GLFW_KEY_F && action == GLFW_PRESS) {
        toggleFullscreen();
    }

    if(key == GLFW_KEY_U && action == GLFW_PRESS) {
        wireframeOn = !wireframeOn;
        glPolygonMode(GL_FRONT_AND_BACK, wireframeOn ? GL_LINE : GL_FILL);
        if(wireframeOn)
            glDisable(GL_CULL_FACE);
        else
            glEnable(GL_CULL_FACE);
    }

    if(key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        simRunning = !simRunning;
    }
}

void Application::mouseScroll(double dx, double dy)
{
    controlSpeed *= 1.0 + dy * 0.1;
}

Application::~Application()
{
    LOG_DEBUG("Destroying application!");
}
