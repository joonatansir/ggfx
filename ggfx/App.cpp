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
    Log::error("OPENGL ERROR id %d: ", id);
    Log::error(message);
    Log::error("\n");
}

App::App(uint32 width, uint32 height, const char* title) :
    window(new Window(width, height, title))
{
    init();
}

App::App() :
    window(new Window(640, 480, "ggfx"))
{
    init();
}

App::~App()
{
    delete window;
}

void App::init()
{
    int32 failure = gl3wInit();
    assert(!failure);
    glDebugMessageCallback(&printCallback, 0);
    Input::Init(window);

    glm::ivec2 size = window->getSize();
    glViewport(0, 0, size.x, size.y);
}

void App::update(float dt)
{
    Input::frameStart(window);
}

void App::render()
{
    window->swapBuffers();
    window->pollEvents();
}

void App::run()
{
    this->init();

    while (!window->shouldClose())
    {
        //TODO: fix my timestep
        float time = (float)glfwGetTime();
        static float lastFrameTime = time;
        float dt = time - lastFrameTime;
        lastFrameTime = time;

        App::update(dt);
        this->update(dt);
        Input::frameEnd(window);
        App::render();
    }
}

double App::getTime()
{
    return glfwGetTime();
}
