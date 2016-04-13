#include <GL/gl3w.h>

#include "VertexBuffer.h"

using namespace ggfx;

VertexBuffer::VertexBuffer(uint32 size, void* data, GLenum usage) :
    GPUBuffer(GL_ARRAY_BUFFER, size, data, usage)
{
}

void VertexBuffer::enableVexterAttribute(GLuint index, GLint components, GLenum type, GLboolean normalized, GLsizei stride, const void* offset)
{
    bind();

    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, components, type, normalized, stride, offset);

    unbind();
}