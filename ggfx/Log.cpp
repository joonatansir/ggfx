#include <stdarg.h>

#include "Log.h"
#include "types.h"

using namespace ggfx;

Log* Log::instance = nullptr;

Log::Log()
{
}

void Log::print(const char* str, ...)
{
    Log* log = getInstance();

    va_list args;
    va_start(args, str);
    log->buffer.appendv(str, args);
    va_end(args);

    log->scrollToBottom = true;
}

void Log::draw()
{
    Log* log = getInstance();

    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Log");

    if (ImGui::Button("Clear"))
    {
        log->buffer.clear();
    }

    ImGui::Separator();
    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::TextUnformatted(log->buffer.begin());

    if (log->scrollToBottom)
        ImGui::SetScrollHere(1.0f);
    log->scrollToBottom = false;

    ImGui::EndChild();
    ImGui::End();
}

Log* Log::getInstance()
{
    if (instance == nullptr)
        instance = new Log();
    return instance;
}
