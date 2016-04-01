#include <assert.h>
#include <stdio.h>

#include "App.h"
#include "Log.h"
#include "Input.h"

using namespace ggfx;

static void APIENTRY printCallback(
    GLenum source, 
    GLenum type, 
    GLuint id, 
    GLenum severity, 
    GLsizei length, 
    const GLchar* message, 
    const void* userParam) 
{ 
    Log::print("OpenGL message: source 0x%04X, type 0x%04X, " "id %u, severity 0x%0X, ’%s’", source, type, id, severity, message); 
}

App::App(Window* window) :
    window(window),
    ui(window)
{
    init();
}

App::~App()
{
}

void App::init()
{
    int32 failure = gl3wInit();
    assert(!failure);

    glDebugMessageCallback(&printCallback, 0);

    Input::Init(window);
}

void App::update()
{
    Input::update(window);
    ui.update(window);
}

void App::render()
{
    ui.render(ImGui::GetDrawData());
    window->swapBuffers();
    window->pollEvents();
}
