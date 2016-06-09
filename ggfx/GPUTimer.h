#pragma once

#include <GL/gl3w.h>

#include "types.h"

namespace ggfx
{
    class GPUTimer
    {
    public:

        GLuint queryBegin = 0;
        GLuint queryEnd = 0;
        float timeElapsed = 0.0f;

        void begin();
        void end();
        float getTime();

    private:

        void create();
    };
}