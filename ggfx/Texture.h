#pragma once

#include <GL/gl3w.h>

#include "types.h"

namespace ggfx
{
    class Texture
    {
    public:

        ~Texture();

        void bind(uint32 activeUnit);

        static Texture createFromFile(const char* filename, uint32 type = GL_TEXTURE_2D, uint32 format = GL_RGBA);

        uint32 type;
        uint32 id;

    private:

        Texture(uint32 type);

        void create2D(uint8* data, uint32 x, uint32 y, uint32 format);
        void createCube(uint8* data[6], uint32 x, uint32 y, uint32 format);
    };
}