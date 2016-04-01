#include "Texture.h"
#include "util.h"

using namespace ggfx;

Texture::Texture(uint32 type):
    type(type),
    id(0)
{
}

Texture::~Texture()
{
}

void Texture::bind(uint32 activeUnit)
{
    glActiveTexture(activeUnit);
    glBindTexture(type, id);
}

Texture Texture::createFromFile(const char* filename, uint32 type, uint32 format)
{
    Texture texture(type);

    int32 x, y, n;
    uint8* imageData = loadImage(filename, &x, &y, &n);

    texture.create2D(imageData, x, y, format);

    freeImageData(imageData);

    return texture;
}

void Texture::create2D(uint8* data, uint32 x, uint32 y, uint32 format)
{
    glGenTextures(1, &id);
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
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    for (uint32 i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data[i]);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}