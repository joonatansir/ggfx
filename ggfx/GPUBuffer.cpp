#include <GL/gl3w.h>

#include "GPUBuffer.h"

using namespace ggfx;

GPUBuffer::GPUBuffer(BufferTarget target) :
    id(0),
    target(getGLenumFromBufferTarget(target))
{ };

GPUBuffer::GPUBuffer(void* data, uint32 size, BufferTarget target, GLenum usage) :
    data((uint8*)data),
    target(getGLenumFromBufferTarget(target))
{
    switch (target)
    {
    case BufferTarget::Persistent:
        createPersistent(size);
        break;
    default:
        create(data, size, usage);
        break;
    }
}

GLenum GPUBuffer::getGLenumFromBufferTarget(BufferTarget target)
{
    switch (target)
    {
    case BufferTarget::Index:
        return GL_ELEMENT_ARRAY_BUFFER;
    default:
        return GL_ARRAY_BUFFER;
    }
}

void GPUBuffer::create(void* data, uint32 size, GLenum usage)
{
    this->data = (uint8*)data;
    glGenBuffers(1, &id);
    glBindBuffer(target, id);
    glBufferData(target, size, data, usage);
    glBindBuffer(target, 0);
}

void GPUBuffer::createPersistent(uint32 size, uint32 flags)
{
    glGenBuffers(1, &id);
    glBindBuffer(target, id);

    glBufferStorage(target, size, NULL, flags);
    data = (uint8 *)glMapBufferRange(target, 0, size, flags);

    glBindBuffer(target, 0);
}

void GPUBuffer::bind()
{
    glBindBuffer(target, id);
}

void GPUBuffer::unbind()
{
    glBindBuffer(target, 0);
}

void GPUBuffer::enableVexterAttribute(GLuint index, GLint components, GLenum type, GLboolean normalized, GLsizei stride, const void* offset)
{
    bind();
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, components, type, normalized, stride, offset);
    unbind();
}