#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "types.h"

namespace ggfx
{
    class GPUBuffer
    {
    public:

        virtual ~GPUBuffer();

        void bind();
        void unbind();
        
        static GPUBuffer* create(uint32 type, uint32 size, void* data, uint32 usage);

        uint32 id;
        uint32 size;
        uint32 type;

    protected:

        GPUBuffer(uint32 type, uint32 size);
        
        void create(void* data, uint32 usage);
    };
}