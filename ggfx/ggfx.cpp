//
//  ggfx.cpp
//  ggfx
//
//  Created by jon on 19/03/16.
//  Copyright © 2016 Joonatan Sörensen. All rights reserved.
//

#include <cassert>
#include <cstdio>

#include "GL/gl3w.h"

#include "ggfx.h"
#include "util.h"

namespace ggfx
{
    GLFWwindow* createWindow(uint32 width, uint32 height, const char* windowTitle)
    {
        int32 success = glfwInit();
        assert(success);
        
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        
        GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);
        assert(window);
        
        glfwMakeContextCurrent(window);
        
        int32 failure = gl3wInit();
        assert(!failure);
        
        printf("%s - %s, %s, %s\n", glGetString(GL_VERSION),
                                    glGetString(GL_RENDERER),
                                    glGetString(GL_VENDOR),
                                    glGetString(GL_SHADING_LANGUAGE_VERSION));
        
        uint32 vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        return window;
    }
    
    uint32 createShaderProgram(uint32 type, const uint8* source)
    {
        return glCreateShaderProgramv(type, 1, (const char**)&source);
    }
    
    uint32 createProgramPipeline(uint32 vertexProgram, uint32 fragmentProgram)
    {
        char buffer[512];
        int32 length;
        glGetProgramInfoLog(vertexProgram, 512, &length, buffer);
        if(length != 0)
            printf("LOG_VERTEX: %s", buffer);
        
        char buffer2[512];
        glGetProgramInfoLog(fragmentProgram, 512, &length, buffer2);
        if(length != 0)
            printf("LOG_FRAGMENT: %s", buffer);
        
        uint32 pipeline;
        glGenProgramPipelines(1, &pipeline);
        glBindProgramPipeline(pipeline);

        glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vertexProgram);
        glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fragmentProgram);
        
        glBindProgramPipeline(0);
        
        return pipeline;
    }
    
    buffer createBuffer(void* data, uint32 type, uint32 size)
    {
        buffer newBuffer = {};
        newBuffer.type = type;
        newBuffer.size = size;
        
        glGenBuffers(1, &newBuffer.id);
        glBindBuffer(newBuffer.type, newBuffer.id);
        
        glBufferData(newBuffer.type, size, data, GL_STATIC_DRAW);
        
        return newBuffer;
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
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        
        glBindTexture(GL_TEXTURE_2D, 0);
        
        freeImageData(imageData);
        
        return newTexture;
    }
    
    void draw(uint32 pipeline)
    {
        glBindProgramPipeline(pipeline);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glBindProgramPipeline(0);
    }
}