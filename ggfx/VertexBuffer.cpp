#include <GL/gl3w.h>

#include "VertexBuffer.h"

using namespace ggfx;

VertexBuffer::VertexBuffer(uint32 type, uint32 size) :
    GPUBuffer(type, size)
{
}

VertexBuffer* VertexBuffer::create(uint32 size, void * data, uint32 usage)
{
    VertexBuffer* buffer = new VertexBuffer(GL_ARRAY_BUFFER, size);
    buffer->GPUBuffer::create(data, usage);
    return buffer;
}

void VertexBuffer::enableVexterAttribute(uint32 location, int32 components, uint32 type, bool normalized, uint32 stride, uint32 offset)
{
    bind();

    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, components, type, normalized, stride, (void *)offset);

    unbind();
}