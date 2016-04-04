#include "Input.h"
#include "GLFWWindow.h"
#include "Log.h"

using namespace ggfx;

glm::vec2 Input::scrollOffset = glm::vec2(0.0f, 0.0f);
glm::vec2 Input::mousePosition = glm::vec2(0.0f, 0.0f);
Input::MouseButtonState Input::mouseButtons[] = { Input::MouseButtonState::UP, Input::MouseButtonState::UP , Input::MouseButtonState::UP };

static void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    Input::scrollOffset = glm::vec2(xOffset, yOffset);
}

static void mouseButtonCallback(GLFWwindow* window, int32 button, int32 action, int32 modifiers)
{
    //TODO: implement
}

void Input::Init(Window* window)
{
    glfwSetScrollCallback(window->handle->ptr, mouseScrollCallback);
    glfwSetMouseButtonCallback(window->handle->ptr, mouseButtonCallback);
}

void Input::frameStart(Window* window)
{
    double x, y;
    glfwGetCursorPos(window->handle->ptr, &x, &y);
    mousePosition.x = (float)x;
    mousePosition.y = (float)y;
}

void Input::frameEnd(Window* window)
{
    scrollOffset.x = 0;
    scrollOffset.y = 0;
}
