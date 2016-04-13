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

        GPUBuffer(GLenum type, uint32 size, void* data, GLenum usage);
        virtual ~GPUBuffer();

        void bind();
        void unbind();
        void update(GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
        
        GLuint id;
        GLenum type;
        uint32 size;

    protected:

        void create(void* data, GLenum usage);
    };
}