#include <assert.h>

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "Window.h"
#include "GLFWWindow.h"

using namespace ggfx;

Window::Window(uint32 width, uint32 height, const char * title)
{
    handle = new WindowHandle();

    int32 success = glfwInit();
    assert(success);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    handle->ptr = glfwCreateWindow(width, height, title, NULL, NULL);
    assert(handle->ptr);

    glfwMakeContextCurrent(handle->ptr);
    glfwSwapInterval(1);
}

Window::~Window()
{
    glfwTerminate();
    delete handle;
}

int32 Window::shouldClose()
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

glm::ivec2 Window::getSize()
{
    int32 w, h;
    glfwGetFramebufferSize(handle->ptr, &w, &h);
    return glm::ivec2(w, h);
}
