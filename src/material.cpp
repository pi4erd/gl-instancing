#include "material.hpp"
#include "log.hpp"

#include <fmt/format.h>

#include <glm/gtc/type_ptr.hpp>

Material::Material(GLuint handle) : program(handle)
{
    int uniformCount;

    glUseProgram(program);
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniformCount);

    GLenum type;
    GLuint location;
    GLsizei size, length;

    char nameBuffer[256];
    for(int i = 0; i < uniformCount; i++) {
        glGetActiveUniform(program, (GLuint)i, sizeof(nameBuffer), &length, &size,
            &type, nameBuffer);
        location = glGetUniformLocation(program, nameBuffer);
        
        MaterialProperty property = {location, type};
        uniforms.insert({nameBuffer, property});
    }

    LOG_DEBUG("Constructed material {}", program);
}

Material::~Material()
{
    glDeleteProgram(program);

    LOG_DEBUG("Destroyed material {}", program);
}

void Material::use()
{
    glUseProgram(program);
}

void Material::uniform1(const std::string &name, GLint value)
{
    glUniform1i(getLocation(name), value);
}

void Material::uniform1(const std::string &name, GLuint value)
{
    glUniform1ui(getLocation(name), value);
}

void Material::uniform1(const std::string &name, GLfloat value)
{
    glUniform1f(getLocation(name), value);
}

void Material::uniform2(const std::string &name, glm::vec2 vec)
{
    glUniform2fv(getLocation(name), 1, glm::value_ptr(vec));
}

void Material::uniform3(const std::string &name, glm::vec3 vec)
{
    glUniform3fv(getLocation(name), 1, glm::value_ptr(vec));
}

void Material::uniform4(const std::string &name, glm::vec4 vec)
{
    glUniform4fv(getLocation(name), 1, glm::value_ptr(vec));
}

void Material::uniform4x4(const std::string &name, glm::mat4 matrix)
{
    glUniformMatrix4fv(getLocation(name), 1, false, glm::value_ptr(matrix));
}

GLuint Material::getLocation(const std::string &name)
{
    if(uniforms.find(name) == uniforms.end()) // Fall back if uniform isn't in the map
        return glGetUniformLocation(program, name.c_str());

    return uniforms[name].location;
}

MaterialBuilder::MaterialBuilder()
{
    LOG_DEBUG("Created MaterialBuilder");
}

MaterialBuilder::~MaterialBuilder()
{
    LOG_DEBUG("Destroyed MaterialBuilder");
}

std::shared_ptr<Material> MaterialBuilder::buildMaterial()
{
    GLuint program = glCreateProgram();

    for(auto &shader : attachedShaders) {
        glAttachShader(program, shader->getId());
    }

    glLinkProgram(program);

    int success;

    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if(success != GL_TRUE) {
        char infoLog[512];
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        throw std::runtime_error(fmt::format("Failed to link shaders in material: {}", infoLog));
    }

    for(auto shader : attachedShaders) {
        glDetachShader(program, shader->getId());
    }
    attachedShaders.clear();

    return std::make_shared<Material>(program);
}

MaterialBuilder MaterialBuilder::attachShader(std::shared_ptr<Shader> shader)
{
    LOG_DEBUG("Attached shader {}", shader->getId());

    attachedShaders.push_back(shader);
    return std::move(*this);
}
