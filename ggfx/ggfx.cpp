//
//  ggfx.cpp
//  ggfx
//
//  Created by jon on 19/03/16.
//  Copyright © 2016 Joonatan Sörensen. All rights reserved.
//

#include <cassert>
#include <cstdio>

#include <GL/gl3w.h>
#include <imgui/imgui.h>

#include "ggfx.h"
#include "util.h"

namespace ggfx
{
    uint32 createShaderProgram(uint32 type, const uint8* source)
    {
        return glCreateShaderProgramv(type, 1, (const char**)&source);
    }

    uint32 createShaderProgramFromFile(const char * filename, uint32 type)
    {
        const uint8* source = loadFile(filename);
        uint32 program = createShaderProgram(type, source);
        delete[] source;
        return program;
    }
    
    uint32 createProgramPipeline(uint32 vertexProgram, uint32 fragmentProgram)
    {        
        uint32 pipeline;
        glGenProgramPipelines(1, &pipeline);
        glBindProgramPipeline(pipeline);

        glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vertexProgram);
        glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fragmentProgram);
        
        glBindProgramPipeline(0);
        
        return pipeline;
    }
}