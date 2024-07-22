#pragma once

#include <GL/glew.h>
#include <memory>

class Texture {
public:
    Texture(GLuint handle);
    ~Texture();

    static std::shared_ptr<Texture> createEmpty(GLsizei width, GLsizei height);

    // Binds and sets texture to unit
    void bind_image(GLuint unit, GLenum accessMode);
    void use(GLuint unit);
    GLuint getHandle() { return handle; }

private:
    GLuint handle;
};
