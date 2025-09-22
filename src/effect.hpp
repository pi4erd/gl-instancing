#pragma once

#include "material.hpp"
#include "mesh.hpp"

class Effect {
public:
    Effect(std::weak_ptr<Shader> fragmentShader);
    ~Effect();

    void apply();

    std::weak_ptr<Material> getMaterial() { return material; }
    
private:
    std::shared_ptr<Material> material;
};
