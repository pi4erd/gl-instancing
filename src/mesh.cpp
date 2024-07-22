#include "mesh.hpp"

#include "log.hpp"

#include <GL/gl.h>
#include <GL/glext.h>
#include <cstddef>
#include <memory>

Mesh::Mesh(GLuint vbo, GLuint vao, GLuint ebo, size_t vcount) : vbo(vbo), vao(vao), ebo(ebo), elementCount(vcount)
{
    LOG_DEBUG("Created mesh {}/{} for {}", vbo, ebo, vao);
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    LOG_DEBUG("Deleted mesh {}/{} for {}", vbo, ebo, vao);
}

void Mesh::draw()
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, 0);
}

void Mesh::drawInstanced(size_t instanceCount)
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElementsInstanced(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, nullptr, instanceCount);
}

std::shared_ptr<Mesh> Mesh::createFromVertexArrayInstanced(
    const std::vector<float> &vertData,
    const std::vector<GLuint> &indices,
    GLuint instanceBuffer) {
    GLuint vbo, vao, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertData.size() * sizeof(float), vertData.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    
    size_t offset = 0;
    size_t sum = 0;
    for(size_t i = 0; i < sizeof(VERTEX_ATTRIBS) / sizeof(int); i++) {
        sum += VERTEX_ATTRIBS[i];
    }
    size_t i;
    for(i = 0; i < sizeof(VERTEX_ATTRIBS) / sizeof(int); i++) {
        glVertexAttribPointer(i, VERTEX_ATTRIBS[i], GL_FLOAT, false, sum * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(i);
        offset += VERTEX_ATTRIBS[i];
    }

    size_t vertexAttribEnd = i;

    offset = 0;
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
    for(i = 0; i < sizeof(INSTANCE_ATTRIBS) / sizeof(int); i++) {
        glVertexAttribPointer(i + vertexAttribEnd, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
        glEnableVertexAttribArray(i + vertexAttribEnd);
        offset += INSTANCE_ATTRIBS[i];
        glVertexAttribDivisor(i + vertexAttribEnd, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glBindVertexArray(0);

    return std::make_shared<Mesh>(vbo, vao, ebo, indices.size());
}

std::shared_ptr<Mesh> Mesh::createFromVertexArray(const std::vector<float> &vertData, const std::vector<GLuint> &indices)
{
    GLuint vbo, vao, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertData.size() * sizeof(float), vertData.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    
    size_t offset = 0;
    size_t sum = 0;
    for(size_t i = 0; i < sizeof(VERTEX_ATTRIBS) / sizeof(int); i++) {
        sum += VERTEX_ATTRIBS[i];
    }
    for(size_t i = 0; i < sizeof(VERTEX_ATTRIBS) / sizeof(int); i++) {
        glVertexAttribPointer(i, VERTEX_ATTRIBS[i], GL_FLOAT, false, sum * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(i);
        offset += VERTEX_ATTRIBS[i];
    }

    glBindVertexArray(0);

    return std::make_shared<Mesh>(vbo, vao, ebo, indices.size());
}
