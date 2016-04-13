#pragma once

#include "GPUBuffer.h"

namespace ggfx
{
    class VertexBuffer : public GPUBuffer
    {
    public:

        VertexBuffer(uint32 size, void* data, GLenum usage);
        void enableVexterAttribute(GLuint index, GLint components, GLenum type, GLboolean normalized, GLsizei stride, const void* offset);
    };
}
