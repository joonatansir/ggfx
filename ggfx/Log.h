#pragma once

#include <imgui/imgui.h>

namespace ggfx
{
    class Log
    {
    public:

        static void print(const char* string, ...);
        static void draw();

    private:

        Log();

        static Log* getInstance();

        static Log* instance;
        ImGuiTextBuffer buffer;
        bool scrollToBottom;
    };
}