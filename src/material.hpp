#pragma once

#include "shader.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>
#include <map>
#include <string>
#include <memory>

struct MaterialProperty {
    GLuint location;
    GLenum type;
};

class Material {
public:
    Material(GLuint handle);
    ~Material();

    void use();
    GLuint getHandle() { return program; }

    void uniform1(const std::string &name, GLint value);
    void uniform1(const std::string &name, GLuint value);
    void uniform1(const std::string &name, GLfloat value);
    void uniform2(const std::string &name, glm::vec2 vec);
    void uniform3(const std::string &name, glm::vec3 vec);
    void uniform4(const std::string &name, glm::vec4 vec);
    void uniform4x4(const std::string &name, glm::mat4 matrix);

private:
    GLuint getLocation(const std::string &name);

    GLuint program;

    std::map<std::string, MaterialProperty> uniforms;
};

class MaterialBuilder {
public:
    MaterialBuilder();
    ~MaterialBuilder();

    std::shared_ptr<Material> buildMaterial();
    MaterialBuilder attachShader(std::shared_ptr<Shader> shader);

private:
    std::vector<std::shared_ptr<Shader>> attachedShaders;
};
