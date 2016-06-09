#pragma once

#include <GL/gl3w.h>

#include "types.h"

namespace ggfx
{
    class GPUTimer
    {
    public:

        ~GPUTimer();

        void begin();
        void end();
        float getTime();

        GLuint queryBegin = 0;
        GLuint queryEnd = 0;
        float timeElapsed = 0.0f;

    private:

        void create();
    };

    #if defined(_DEBUG)
        #define GPU_TIMER_START(name) GPUTimer name##TimeQuery; name##TimeQuery.begin()
        #define GPU_TIMER_END(name) name##TimeQuery.end()
        #define GPU_TIMER_GET(name) name##TimeQuery.getTime()
    #else
        #define GPU_TIMER_START(name)
        #define GPU_TIMER_END(name)
        #define GPU_TIMER_GET(name) 0.0f
    #endif
}