#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "types.h"

namespace ggfx
{
    class Input
    {
        enum class MouseButtonState
        {
            DOWN,
            UP,
        };

    public:

        static void InitInput(GLFWwindow* window);
        static void update(GLFWwindow* window);

        static bool GetMouseButton(int32 button);
        static bool GetMouseButtonDown(int32 button);
        static bool GetMouseButtonUp(int32 button);

        static glm::vec2 mousePosition;
        static glm::vec2 scrollOffset;
        static MouseButtonState mouseButtons[];

    private:

        Input();

    };
}