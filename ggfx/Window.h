#pragma once

#include <glm/vec2.hpp>

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

        int32 shouldClose();
        void pollEvents();
        void swapBuffers();
        glm::ivec2 getSize();

        WindowHandle* handle;
    };
}