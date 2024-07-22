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

#define RANDF(MIN, MAX) (static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (MAX - MIN))) + MIN)

Application::Application() 
    : Window("My window"), imguiInstance(getWindow()), cameraRotation(0.0) {
    std::srand(time(nullptr));

    glViewport(0, 0, width, height);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    const GLubyte *version = glGetString(GL_VERSION);
    LOG_INFO("Version info: OpenGL {}", (const char*)version);

    glfwGetCursorPos(getWindow(), &prevMouseX, &prevMouseY);
    
    cubePositions = generateRandomPositions(1000000, -200.0, 200.0);

    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * cubePositions->size(), cubePositions->data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    cubeMesh = Mesh::createFromVertexArrayInstanced(
    { // vertices
        -1, -1, -1,
        1, -1, -1,
        1, 1, -1,
        -1, 1, -1,
        
        -1, -1, 1,
        1, -1, 1,
        1, 1, 1,
        -1, 1, 1,
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

    while(!shouldClose()) {
        pollEvents();

        double time = glfwGetTime();
        double deltaTime = time - prevTime;
        prevTime = time;

        update(deltaTime);
        render(deltaTime);
    }
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
    ImGui::End();

    imguiInstance.renderFrame();
}

std::unique_ptr<std::vector<glm::vec3>> Application::generateRandomPositions(size_t size, float min, float max)
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
    } else {
        glfwSetInputMode(getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
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
}

void Application::mouseScroll(double dx, double dy)
{
    controlSpeed *= 1.0 + dy * 0.1;
}

Application::~Application()
{
    LOG_DEBUG("Destroying application!");
}
