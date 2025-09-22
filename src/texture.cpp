#include "texture.hpp"
#include "log.hpp"

Texture::Texture(GLuint handle) : handle(handle) {
    LOG_DEBUG("Created texture {}", handle);
}

std::shared_ptr<Texture> Texture::createEmptyStorage(GLsizei width, GLsizei height) {
    GLuint handle;

    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);

    glTextureParameteri(handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureStorage2D(handle, 1, GL_RGBA32F, width, height);
    
    return std::make_shared<Texture>(handle);
}

void Texture::recreateStorage(GLsizei newWidth, GLsizei newHeight)
{
    // Hope this is enough
    glBindTexture(GL_TEXTURE_2D, handle);
    glTextureStorage2D(handle, 1, GL_RGBA32F, newWidth, newHeight);
}

Texture::~Texture() {
    LOG_DEBUG("Destroying texture {}", handle);
    glDeleteTextures(1, &handle);
}

void Texture::use(GLuint unit) {
    glBindTextureUnit(unit, handle);
}

void Texture::bind_image(GLuint unit, GLenum accessMode) {
    glBindImageTexture(unit, handle, 0, GL_FALSE, 0, accessMode, GL_RGBA32F);
}
