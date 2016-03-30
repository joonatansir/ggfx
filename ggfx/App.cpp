#include <assert.h>
#include <stdio.h>

#include "App.h"
#include "Log.h"

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


App::App(uint32 width, uint32 height, const char* title)
{
    createWindow(width, height, title);
}

App::~App()
{
}

bool App::shouldClose()
{
    return glfwWindowShouldClose(m_window);
}

void App::pollEvents()
{
    glfwPollEvents();
}

void App::swapBuffers()
{
    glfwSwapBuffers(m_window);
}

GLFWwindow* App::getWindow()
{
    return m_window;
}

void App::createWindow(uint32 width, uint32 height, const char * title)
{
    assert(glfwInit());

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    assert(m_window = glfwCreateWindow(width, height, title, NULL, NULL));

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    assert(!gl3wInit());

    glViewport(0, 0, width, height);

    glDebugMessageCallback(&printCallback, 0);

    //TODO: Remove this from here
    uint32 vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
}
