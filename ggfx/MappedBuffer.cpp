#include <GL/gl3w.h>

#include "MappedBuffer.h"

using namespace ggfx;

MappedBuffer::MappedBuffer(uint32 type, uint32 size) :
    GPUBuffer(type, size)
{
}

MappedBuffer* MappedBuffer::create(uint32 type, uint32 size)
{
    MappedBuffer* buffer = new MappedBuffer(type, size);

    glGenBuffers(1, &buffer->id);
    glBindBuffer(type, buffer->id);

    uint32 flags = GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT;
    glBufferStorage(type, size, NULL, flags);
    buffer->data = (uint8 *)glMapBufferRange(type, 0, size, flags);

    glBindBuffer(type, 0);

    return buffer;
}
