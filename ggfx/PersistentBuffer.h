#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "types.h"

namespace ggfx
{
    class PersistentBuffer
    {
    public:

        PersistentBuffer(uint32 type, uint32 size);

        void create(uint32 type, uint32 size);

        union
        {
            uint8* data;
            int8* asInt8;
            uint16* asUint16;
            int16* asInt16;
            uint32* asUint32;
            int32* asInt32;
            float* asFloat;
            glm::tvec2<float>* asVec2;
            glm::tvec3<float>* asVec3;
            glm::tvec4<float>* asVec4;
        };

        GLuint id;
        GLenum type;
        uint32 size;
    };
}

