#pragma once

#include "App.h"
#include "DebugUI.h"

class PBRApp : public ggfx::App
{
public:
    PBRApp(uint32 width, uint32 height, const char* title);
    ~PBRApp();

    virtual void update(float dt);
    virtual void init();

    ggfx::DebugUI ui;
};

