#pragma once

#include <string>

#include <GL/gl3w.h>

#include "types.h"

namespace ggfx
{
    class Texture
    {
    public:

        struct TextureOptions
        {
            uint32 internalFormat;
            bool flipY;
        };

        ~Texture();

        void bind(uint32 activeUnit);

        static Texture create2DFromFile(const std::string& filename, TextureOptions format = DEFAULT_TEXTURE_OPTIONS);
        static Texture createCubemapFromFile(const std::string filenames[6], TextureOptions format = DEFAULT_TEXTURE_OPTIONS);
        static Texture createCubemapFromFile(
            const std::string& posx, 
            const std::string& negx, 
            const std::string& posy, 
            const std::string& negy, 
            const std::string& posz, 
            const std::string& negz, 
            TextureOptions format = DEFAULT_TEXTURE_OPTIONS);

        uint32 type;
        uint32 id;

    private:

        Texture();
        Texture(uint32 type);

        void create2D(uint8* data, uint32 x, uint32 y, uint32 format);
        void createCube(uint8* data[6], uint32 x, uint32 y, uint32 format);

        static const TextureOptions DEFAULT_TEXTURE_OPTIONS;
    };
}