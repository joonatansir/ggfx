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

        static Texture create2DFromFile(const char* filename, uint32 format = GL_RGBA, bool flipY = true);
        static Texture createCubeFromFile(const char* filenames[6], uint32 format = GL_RGBA, bool flipY = true);
        static Texture createCubeFromFile(const char* posx, const char* negx, const char* posy, const char* negy, const char* posz, const char* negz, uint32 format = GL_RGBA, bool flipY = true);

        uint32 type;
        uint32 id;

    private:

        Texture(uint32 type);

        void create2D(uint8* data, uint32 x, uint32 y, uint32 format);
        void createCube(uint8* data[6], uint32 x, uint32 y, uint32 format);
    };
}