#pragma once

#include <GL/glew.h>

#include <memory>
#include <string>

class Shader {
public:
    Shader(GLenum shaderType);
    ~Shader();

    GLuint getId();

    void compile();

    GLuint shader;
};

std::shared_ptr<Shader> shaderFromGlslFile(std::string path, GLenum shaderType);
std::shared_ptr<Shader> shaderFromBinaryFile(std::string path, GLenum shaderType);
