#include <GL/gl3w.h>

#include "Buffer.h"

using namespace ggfx;

GPUBuffer::GPUBuffer(uint32 type, uint32 size) :
    id(0),
    type(type),
    size(size)
{
}

GPUBuffer::~GPUBuffer()
{
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

void GPUBuffer::createPersistent()
{
    glGenBuffers(1, &id);
    glBindBuffer(type, id);

    uint32 flags = GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT;
    glBufferStorage(type, size, NULL, flags);
    data = (uint8 *)glMapBufferRange(type, 0, size, flags);

    glBindBuffer(type, 0);
}

void GPUBuffer::enableVexterAttribute(uint32 location, int32 components, uint32 type, bool normalized, uint32 stride, uint32 offset)
{
    bind();

    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, components, type, normalized, stride, (void *)offset);

    unbind();
}

GPUBuffer GPUBuffer::create(uint32 type, uint32 size, void * data, uint32 usage)
{
    GPUBuffer buffer(type, size);
    buffer.create(data, usage);
    return buffer;
}

GPUBuffer GPUBuffer::createPersistent(uint32 type, uint32 size)
{
    GPUBuffer buffer(type, size);
    buffer.createPersistent();
    return buffer;
}
