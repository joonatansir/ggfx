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

        ~GPUBuffer();

        void bind();
        void unbind();
        void enableVexterAttribute(uint32 location, int32 components, uint32 type, bool normalized, uint32 stride, uint32 offset);
        
        static GPUBuffer create(uint32 type, uint32 size, void* data, uint32 usage);
        static GPUBuffer createPersistent(uint32 type, uint32 size);

        uint32 id;
        uint32 size;
        uint32 type;

        union
        {
            uint8* data;
            int8* as_int8;
            uint16* as_uint16;
            int16* as_int16;
            uint32* as_uint32;
            int32* as_int32;
            float32* as_float32;
            glm::tvec2<float32>* as_vec2;
            glm::tvec3<float32>* as_vec3;
            glm::tvec4<float32>* as_vec4;
        };

    private:

        GPUBuffer(uint32 type, uint32 size);
        
        void create(void* data, uint32 usage);
        void createPersistent();
    };
}