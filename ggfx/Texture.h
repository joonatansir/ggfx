#pragma once

#include <GL/gl3w.h>

#include "types.h"

namespace ggfx
{
    class Texture
    {
    public:

        struct TextureFormat
        {
            uint32 internalFormat;
            bool flipY;
        };

        Texture();
        ~Texture();

        void bind(uint32 activeUnit);

        static Texture create2DFromFile(const char* filename, TextureFormat format = DEFAULT_TEXTURE_FORMAT);
        static Texture createCubemapFromFile(const char* filenames[6], TextureFormat format = DEFAULT_TEXTURE_FORMAT);
        static Texture createCubemapFromFile(const char* posx, const char* negx, const char* posy, const char* negy, const char* posz, const char* negz, TextureFormat format = DEFAULT_TEXTURE_FORMAT);

        uint32 type;
        uint32 id;

    private:

        Texture(uint32 type);

        void create2D(uint8* data, uint32 x, uint32 y, uint32 format);
        void createCube(uint8* data[6], uint32 x, uint32 y, uint32 format);

        static const TextureFormat DEFAULT_TEXTURE_FORMAT;
    };
}