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
        static void spam(const char* string, ...);
        static void draw();

    private:

        Log();

        static Log* getInstance();
        void drawButton(uint32& selectedBuffer, uint32 bufferId, const char* title);

        ImGuiTextBuffer buffers[3]; //0: info buffer, 1: spam buffer, 2: error buffer
        bool scrollToBottom;
    };

    #if defined(_DEBUG)
        #define LOG(type, message, ...) Log::##type(message, __VA_ARGS__)
    #else
        #define LOG(message, type)
    #endif
}