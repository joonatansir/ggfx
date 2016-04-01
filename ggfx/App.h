#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "types.h"
#include "DebugUI.h"
#include "Window.h"

namespace ggfx
{
    class App
    {
    public:

        App(Window* window);
        ~App();

        void init();
        void update();
        void render();

    private:

        Window* window;
        DebugUI ui;
    };
}