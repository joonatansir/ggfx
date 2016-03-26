#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "types.h"

namespace ggfx
{
    class App
    {
    public:

        App(uint32 width, uint32 height, const char* title);
        ~App();

        bool shouldClose();
        void pollEvents();
        void swapBuffers();
        GLFWwindow* getWindow();

    private:

        void createWindow(uint32 width, uint32 height, const char* title);

        GLFWwindow* m_window;
    };
}