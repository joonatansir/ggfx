#include <assert.h>

#include <GLFW/glfw3.h>

#include "Window.h"
#include "GLFWWindow.h"

using namespace ggfx;

Window::Window(uint32 width, uint32 height, const char * title)
{
    handle = new WindowHandle();

    assert(glfwInit());

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    assert(handle->ptr = glfwCreateWindow(width, height, title, NULL, NULL));

    glfwMakeContextCurrent(handle->ptr);
    glfwSwapInterval(1);
}

Window::~Window()
{
    glfwTerminate();
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(handle->ptr);
}

void Window::pollEvents()
{
    glfwPollEvents();
}

void Window::swapBuffers()
{
    glfwSwapBuffers(handle->ptr);
}
