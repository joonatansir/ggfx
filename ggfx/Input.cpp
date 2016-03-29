#include "Input.h"

using namespace ggfx;

glm::vec2 Input::scrollOffset = glm::vec2(0.0f, 0.0f);
glm::vec2 Input::mousePosition = glm::vec2(0.0f, 0.0f);
Input::MouseButtonState Input::mouseButtons[] = { Input::MouseButtonState::UP, Input::MouseButtonState::UP , Input::MouseButtonState::UP };

static void mouseScrollCallback(GLFWwindow* window, float64 xOffset, float64 yOffset)
{
    Input::scrollOffset = glm::vec2(xOffset, yOffset);
}

static void mouseButtonCallback(GLFWwindow* window, int32 button, int32 action, int32 modifiers)
{
    //TODO: implement
}

void Input::Init(GLFWwindow * window)
{
    glfwSetScrollCallback(window, mouseScrollCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
}

void Input::update(GLFWwindow* window)
{
    float64 x, y;
    glfwGetCursorPos(window, &x, &y);
    mousePosition.x = (float32)x;
    mousePosition.y = (float32)y;

    scrollOffset.x = 0;
    scrollOffset.y = 0;
}
