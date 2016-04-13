#include <GL/gl3w.h>

#include "GPUBuffer.h"

using namespace ggfx;

GPUBuffer::GPUBuffer(GLenum type, uint32 size, void* data, GLenum usage) :
    id(0),
    type(type),
    size(size)
{
    create(data, usage);
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

void GPUBuffer::create(void* data, GLenum usage)
{
    glGenBuffers(1, &id);
    glBindBuffer(type, id);
    glBufferData(type, size, data, usage);
    glBindBuffer(type, 0);
}

void GPUBuffer::update(GLenum target, GLintptr offset, GLsizeiptr size, const void* data)
{
    glBindBuffer(type, id);
    glBufferSubData(target, offset, size, data);
    glBindBuffer(type, 0);
}