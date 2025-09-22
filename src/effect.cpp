#include "effect.hpp"

#include "log.hpp"

Effect::Effect(std::weak_ptr<Shader> fragmentShader)
{
    std::shared_ptr<Shader> defaultVertexShader = 
        shaderFromBinaryFile("shaders/effect.vert.spv", GL_VERTEX_SHADER);

    material = MaterialBuilder()
        .attachShader(defaultVertexShader)
        .attachShader(fragmentShader.lock())
        .buildMaterial();

    LOG_DEBUG("Effect {} created", material->getHandle());
}

Effect::~Effect()
{
    LOG_DEBUG("Effect {} destroying", material->getHandle());
}

void Effect::apply()
{
    material->use();
}
