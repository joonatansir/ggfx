#pragma once

#include <imgui/imgui.h>

#include "types.h"
#include "Window.h"

namespace ggfx
{
    class DebugUI
    {
    public:

        DebugUI(Window* window);
        ~DebugUI();

        void createDebugUI(Window* window);
        void update(Window* window);
        void render(ImDrawData* drawData);
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