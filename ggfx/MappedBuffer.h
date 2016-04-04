#pragma once

#include "GPUBuffer.h"

namespace ggfx
{
    class MappedBuffer : public GPUBuffer
    {
    public:

        static MappedBuffer* create(uint32 type, uint32 size);

        union
        {
            uint8* data;
            int8* as_int8;
            uint16* as_uint16;
            int16* as_int16;
            uint32* as_uint32;
            int32* as_int32;
            float* as_float;
            glm::tvec2<float>* as_vec2;
            glm::tvec3<float>* as_vec3;
            glm::tvec4<float>* as_vec4;
        };

    private:

        MappedBuffer(uint32 type, uint32 size);
    };
}

