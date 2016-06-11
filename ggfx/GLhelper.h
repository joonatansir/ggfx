#pragma once

#include <GL/gl3w.h>

#include "Log.h"

#if defined(_DEBUG)
#define GL_PRINT(name)                          \
            GLint data;                        \
            glGetIntegerv(name, &data);         \
            LOG(info, "%s: %d\n", #name, data);
#endif