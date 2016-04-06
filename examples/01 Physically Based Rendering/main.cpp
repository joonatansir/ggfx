#include <Windows.h>

#include "PBRApp.h"

int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    PBRApp app(1280, 720, "PBR");
    app.run();
}