//
//  ggfx.h
//  ggfx
//
//  Created by jon on 19/03/16.
//  Copyright © 2016 Joonatan Sörensen. All rights reserved.
//

#pragma once

#include "GLFW/glfw3.h"

#include "types.h"

#define GGFX_DEBUG 1

namespace ggfx
{
    struct buffer
    {
        uint32 id;
        void* data;
    };

    GLFWwindow* createWindow(uint32 width, uint32 height, const char* windowTitle);
    
    uint32 createShaderProgram(uint32 type, const char* source);
    
    uint32 createProgramPipeline(uint32 vertexProgram, uint32 fragmentProgram);
    
    buffer createBuffer();
    
    void draw(uint32 pipeline);
}