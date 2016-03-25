#pragma once

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

#include "types.h"

struct debugUI
{
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

void createDebugUI(GLFWwindow* window);
void newDebugUIFrame(GLFWwindow* window);
void renderDebugUI(ImDrawData* drawData);
void createDebugUIObjects();

void mouseClickCallback(GLFWwindow* window, int32 button, int32 action, int32 modifiers);
void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);