#pragma once

#include "types.h"

namespace ggfx
{
    struct WindowHandle;

    class Window
    {
    public:

        Window(uint32 width, uint32 height, const char* title);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool shouldClose();
        void pollEvents();
        void swapBuffers();

        WindowHandle* handle;
    };
}