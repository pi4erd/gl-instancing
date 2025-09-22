#pragma once

#include "effect.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "texture.hpp"

#include <vector>
#include <memory>
#include <cstdint>
#include <cstddef>

class PostProcess {
public:
    PostProcess(GLsizei width, GLsizei height);
    ~PostProcess();

    void beginDraw();
    void endDraw();

    void render();
    void resize(GLsizei width, GLsizei height);

    void addEffect(size_t order, std::unique_ptr<Effect> effect);

    size_t getEffectCount() { return effects.size(); }

private:
    GLuint effectBackbuffer, rbo;

    std::unique_ptr<Effect> blitEffect;
    std::shared_ptr<Texture> framebufferTexture;
    std::shared_ptr<Mesh> effectMesh;

    std::vector<std::unique_ptr<Effect>> effects;
};
