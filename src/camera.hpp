#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    Camera();
    ~Camera();
    
    glm::mat4 viewMatrix();
    glm::mat4 projectionMatrix(float aspect);

public: // public properties
    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec3 up;

    float nearPlane, farPlane;
    float fovY;
};
