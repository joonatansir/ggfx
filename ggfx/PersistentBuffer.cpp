#include <GL/gl3w.h>

#include "PersistentBuffer.h"

using namespace ggfx;

PersistentBuffer::PersistentBuffer(uint32 type, uint32 size)
{
    create(type, size);
}

void PersistentBuffer::create(uint32 type, uint32 size)
{
    glGenBuffers(1, &id);
    glBindBuffer(type, id);

    uint32 flags = GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT;
    glBufferStorage(type, size, NULL, flags);
    data = (uint8 *)glMapBufferRange(type, 0, size, flags);

    glBindBuffer(type, 0);
}
