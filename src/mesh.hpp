#pragma once

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include <memory>

constexpr int VERTEX_ATTRIBS[] = {
    3, // v_position
};

constexpr int INSTANCE_ATTRIBS[] = {
    3, // i_offset
};

class Mesh {
public:
    Mesh(GLuint vbo, GLuint vao, GLuint ebo, size_t vcount);
    ~Mesh();

    void draw();
    void drawInstanced(size_t instanceCount);

    static std::shared_ptr<Mesh> createFromVertexArray(
        const std::vector<float> &vertData,
        const std::vector<GLuint> &indices
    );
    static std::shared_ptr<Mesh> createFromVertexArrayInstanced(
        const std::vector<float> &vertData,
        const std::vector<GLuint> &indices,
        GLuint instanceBuffer
    );

private:
    GLuint vbo, vao, ebo;
    
    size_t elementCount;
};
