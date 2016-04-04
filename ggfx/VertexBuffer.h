#pragma once

#include "GPUBuffer.h"

namespace ggfx
{
    class VertexBuffer : public GPUBuffer
    {
    public:

        static VertexBuffer* create(uint32 size, void* data, uint32 usage);

        void enableVexterAttribute(uint32 location, int32 components, uint32 type, bool normalized, uint32 stride, uint32 offset);
    
    private:

        VertexBuffer(uint32 type, uint32 size);
    };
}
