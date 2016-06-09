#include <stdarg.h>

#include "Log.h"

using namespace ggfx;

Log::Log()
{
}

void Log::info(const char* str, ...)
{
    Log* log = getInstance();

    va_list args;
    va_start(args, str);
    log->buffers[0].appendv(str, args);
    va_end(args);

    log->scrollToBottom = true;
}

void Log::error(const char* str, ...)
{
    Log* log = getInstance();

    va_list args;
    va_start(args, str);
    log->buffers[2].appendv(str, args);
    va_end(args);

    log->scrollToBottom = true;
}

void Log::spam(const char * str, ...)
{
    Log* log = getInstance();

    va_list args;
    va_start(args, str);
    log->buffers[1].appendv(str, args);
    va_end(args);

    log->scrollToBottom = true;
}

void Log::drawButton(uint32& selectedBuffer, uint32 bufferId, const char* title)
{
    bool selected = selectedBuffer == bufferId;

    if(selected)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 0.3f));

    if (ImGui::Button(title))
    {
        selectedBuffer = bufferId;
    }

    if (selected)
        ImGui::PopStyleColor();
}

void Log::draw()
{
    Log* log = getInstance();

    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Log");

    static uint32 selectedBuffer = 0;

    log->drawButton(selectedBuffer, 0, "Info");
    ImGui::SameLine();
    log->drawButton(selectedBuffer, 1, "Warning");
    ImGui::SameLine();
    log->drawButton(selectedBuffer, 2, "Error");
    ImGui::SameLine();

    if (ImGui::Button("Clear"))
    {
        log->buffers[selectedBuffer].clear();
    }

    ImGui::Separator();

    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::TextUnformatted(log->buffers[selectedBuffer].begin());

    if (log->scrollToBottom)
        ImGui::SetScrollHere(1.0f);

    log->scrollToBottom = false;

    ImGui::EndChild();
    ImGui::End();
}

Log* Log::getInstance()
{
    static Log* instance = new Log();
    return instance;
}
