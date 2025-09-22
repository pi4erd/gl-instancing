#include "postprocess.hpp"

#include <GL/gl.h>
#include <stdexcept>

PostProcess::PostProcess(GLsizei width, GLsizei height)
{
    effectMesh = Mesh::createFromVertexArrayAttrib({
        -1, -1, 0, 0,
         1, -1, 1, 0,
         1,  1, 1, 1,
        -1,  1, 0, 1
    }, {
        0, 1, 2,
        0, 2, 3
    }, {
        2, 2
    });

    glGenFramebuffers(1, &effectBackbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, effectBackbuffer);

    framebufferTexture = Texture::createEmptyStorage(width, height);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        framebufferTexture->getHandle(), 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Framebuffer was not complete!");
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::shared_ptr<Shader> blitShaderFrag = shaderFromBinaryFile("shaders/framebufferblit.frag.spv", GL_FRAGMENT_SHADER);
    
    blitEffect = std::make_unique<Effect>(blitShaderFrag);
}

PostProcess::~PostProcess()
{
    glDeleteFramebuffers(1, &effectBackbuffer);
    glDeleteRenderbuffers(1, &rbo);
}

void PostProcess::beginDraw()
{
    glBindFramebuffer(GL_FRAMEBUFFER, effectBackbuffer);
}

void PostProcess::endDraw()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcess::render()
{
    glDisable(GL_DEPTH_TEST);
    for(auto &effect : effects) {
        effect->apply();
        effect->getMaterial().lock()->uniform1("renderTexture", 0);
        effectMesh->draw();
    }

    blitEffect->apply();
    blitEffect->getMaterial().lock()->uniform1("renderTexture", 0);
    effectMesh->draw();
    glEnable(GL_DEPTH_TEST);
}

void PostProcess::resize(GLsizei width, GLsizei height)
{
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    framebufferTexture->recreateStorage(width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void PostProcess::addEffect(size_t order, std::unique_ptr<Effect> effect)
{
    if(order > effects.size()) {
        effects.push_back(std::move(effect));
        return;
    }

    effects.insert(effects.begin() + order, std::move(effect));
}
