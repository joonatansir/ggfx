#include <Windows.h>

#include "PBRApp.h"

int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    PBRApp app(1600, 900, "PBR");
    app.run();
}