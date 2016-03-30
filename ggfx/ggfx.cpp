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
    
    texture createTextureFromFile(const char* filename)
    {
        texture newTexture = {};
        
        int32 x, y, n;
        uint8* imageData = loadImage(filename, &x, &y, &n);
        
        assert(imageData);
        
        glGenTextures(1, &newTexture.id);
        glBindTexture(GL_TEXTURE_2D, newTexture.id);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        
        glBindTexture(GL_TEXTURE_2D, 0);
        
        freeImageData(imageData);
        
        return newTexture;
    }
    
    void draw(uint32 pipeline)
    {
        glBindProgramPipeline(pipeline);
        
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
        
        //glBindProgramPipeline(0);
    }
}