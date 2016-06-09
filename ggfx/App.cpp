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
    LOG(error, "OPENGL ERROR id %d: ", id);
    LOG(error, message);
    LOG(error, "\n");
}

App::App(uint32 width, uint32 height, const char* title) :
    window(new Window(width, height, title))
{
    int32 failure = gl3wInit();
    assert(!failure);
    glDebugMessageCallback(&printCallback, 0);
    Input::Init(window);

    glm::ivec2 size = window->getSize();
    glViewport(0, 0, size.x, size.y);
}

App::App() :
    App(640, 480, "ggfx")
{
}

App::~App()
{
    delete window;
}

void App::run()
{
    this->init();

    float dt = 0.016f;
    float accumulator = 0.0f;

    while (!window->shouldClose())
    {
        float currentFrameTime = (float)glfwGetTime();
        static float lastFrameTime = currentFrameTime;
        float frametime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        accumulator += frametime;

        while (accumulator >= dt)
        {
            Input::frameStart(window);
            this->update(dt);
            Input::frameEnd(window);

            accumulator -= dt;
        }
        
        this->render(frametime);

        window->swapBuffers();
        window->pollEvents();
    }
}

double App::getTime()
{
    return glfwGetTime();
}
