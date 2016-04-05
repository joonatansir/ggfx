#pragma once

#include <imgui/imgui.h>

#include "types.h"

namespace ggfx
{
    class Log
    {
    public:

        static void info(const char* string, ...);
        static void error(const char* string, ...);
        static void warning(const char* string, ...);
        static void draw();

    private:

        Log();

        static Log* getInstance();
        void drawButton(uint32& selectedBuffer, uint32 bufferId, const char* title);

        ImGuiTextBuffer buffers[3]; //0: info buffer, 1: warning buffer, 2: error buffer
        bool scrollToBottom;
    };
}