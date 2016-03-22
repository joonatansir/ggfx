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

namespace ggfx
{
    struct buffer
    {
        uint32 id;
        uint32 type;
        uint32 size;
    };

    struct texture
    {
        uint32 id;
    };
    
    GLFWwindow* createWindow(uint32 width, uint32 height, const char* windowTitle);
    
    uint32 createShaderProgram(uint32 type, const uint8* source);
    
    uint32 createProgramPipeline(uint32 vertexProgram, uint32 fragmentProgram);
    
    buffer createBuffer(void* data, uint32 type, uint32 size);
    
    texture createTextureFromFile(const char* filename);
    
    void draw(uint32 pipeline);
}