#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <cmath>

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>

#include "ggfx.h"
#include "util.h"
#include "DebugUI.h"
#include "resources.h"
#include "App.h"
#include "Input.h"
#include "GPUBuffer.h"
#include "Log.h"
#include "Texture.h"
#include "Window.h"
#include "GLFWWindow.h"
#include "ShaderPipeline.h"
#include "VertexBuffer.h"
#include "MyApp.h"

using namespace ggfx;



int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    uint32 width = 1600;
    uint32 height = 900;

    MyApp myapp(width, height, "ggfx");
    myapp.run();

    return 0;
}