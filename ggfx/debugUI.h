#pragma once

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

#include "types.h"

namespace ggfx
{
    class DebugUI
    {
    public:

        DebugUI(GLFWwindow* window);
        ~DebugUI();

        void createDebugUI(GLFWwindow* window);
        void newFrame(GLFWwindow* window);
        void renderDebugUI(ImDrawData* drawData);
        void createDebugUIObjects();

    private:

        uint32 fontTexture;
        uint32 shaderHandle;
        uint32 vertHandle;
        uint32 fragHandle;
        int32 attribLocationTex;
        int32 attribLocationProjMtx;
        int32 attribLocationPosition;
        int32 attribLocationUV;
        int32 attribLocationColor;
        uint32 vboHandle;
        uint32 vaoHandle;
        uint32 elementsHandle;
    };
}