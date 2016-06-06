#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "types.h"

namespace ggfx
{
    enum class BufferTarget
    {
        Array,
        Index,
        Persistent,
    };

    class GPUBuffer
    {
    public:

        explicit GPUBuffer(BufferTarget target);
        GPUBuffer(void* data, uint32 size, BufferTarget target = BufferTarget::Array, GLenum usage = GL_STATIC_DRAW);

        GLenum getGLenumFromBufferTarget(BufferTarget target);
        void create(void* data, uint32 size, GLenum usage = GL_STATIC_DRAW);
        void createPersistent(uint32 size, uint32 flags = GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT);
        void bind();
        void unbind();
        void enableVexterAttribute(GLuint index, GLint components, GLenum type, GLboolean normalized, GLsizei stride, const void* offset);

        GLuint id;
        GLenum target;

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
    };
}