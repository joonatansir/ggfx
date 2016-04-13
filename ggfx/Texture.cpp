#include "Texture.h"
#include "util.h"
#include "Log.h"

using namespace ggfx;

const Texture::TextureOptions Texture::DEFAULT_TEXTURE_OPTIONS = { GL_RGBA, true };

Texture::Texture(uint32 type):
    type(type),
    id(0)
{
}

Texture::Texture()
{
}

Texture::~Texture()
{
}

void Texture::bind(uint32 activeUnit)
{
    glActiveTexture(GL_TEXTURE0 + activeUnit);
    glBindTexture(type, id);
}

Texture Texture::create2DFromFile(const std::string& filename, TextureOptions format)
{
    Texture texture(GL_TEXTURE_2D);

    int32 x, y, n;
    uint8* imageData = loadImage(filename, &x, &y, &n, format.flipY);

    texture.create2D(imageData, x, y, format.internalFormat);

    freeImageData(imageData);

    return texture;
}

Texture Texture::createCubemapFromFile(const std::string filenames[6], TextureOptions format)
{
    Texture texture(GL_TEXTURE_CUBE_MAP);

    int32 x, y, n;
    uint8* data[6];
    for (int32 i = 0; i < 6; i++)
    {
        data[i] = loadImage(filenames[i], &x, &y, &n, format.flipY);
    }

    texture.createCube(data, x, y, format.internalFormat);

    for (int32 i = 0; i < 6; i++)
    {
        freeImageData(data[i]);
    }

    return texture;
}

Texture ggfx::Texture::createCubemapFromFile(
    const std::string& posx,
    const std::string& negx,
    const std::string& posy,
    const std::string& negy,
    const std::string& posz,
    const std::string& negz, 
    TextureOptions format)
{
    const std::string filenames[] =
    {
        posx, negx,
        posy, negy,
        posz, negz,
    };

    return createCubemapFromFile(filenames, format);
}

void Texture::create2D(uint8* data, uint32 x, uint32 y, uint32 format)
{
    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, format, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::createCube(uint8 * data[6], uint32 x, uint32 y, uint32 format)
{
    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    for (int32 i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data[i]);
    }

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
