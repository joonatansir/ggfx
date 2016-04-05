#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "types.h"
#include "Window.h"

namespace ggfx
{
    class App
    {
    public:

        App();
        App(uint32 width, uint32 height, const char* title);
        virtual ~App();

        virtual void update(float dt) = 0;
        virtual void init() = 0;
        void run();

        double getTime();

    protected:

        Window* window;
    };
}