#include <assert.h>
#include <stdio.h>

#include "App.h"

using namespace ggfx;

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

    assert(m_window = glfwCreateWindow(width, height, title, NULL, NULL));

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    assert(!gl3wInit());

    glViewport(0, 0, width, height);

    //TODO: Remove this from here
    uint32 vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
}
