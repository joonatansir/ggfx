#include <GL/gl3w.h>

#include "GPUBuffer.h"

using namespace ggfx;

GPUBuffer::GPUBuffer(uint32 type, uint32 size) :
    id(0),
    type(type),
    size(size)
{
}

GPUBuffer::~GPUBuffer()
{
    //TODO: delete buffers here
}

void GPUBuffer::bind()
{
    glBindBuffer(type, id);
}

void GPUBuffer::unbind()
{
    glBindBuffer(type, 0);
}

void GPUBuffer::create(void* data, uint32 usage)
{
    glGenBuffers(1, &id);
    glBindBuffer(type, id);
    glBufferData(type, size, data, usage);
    glBindBuffer(type, 0);
}

GPUBuffer* GPUBuffer::create(uint32 type, uint32 size, void * data, uint32 usage)
{
    GPUBuffer* buffer = new GPUBuffer(type, size);
    buffer->create(data, usage);
    return buffer;
}