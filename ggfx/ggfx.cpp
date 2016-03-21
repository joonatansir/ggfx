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

using namespace ggfx;

GLFWwindow* ggfx::createWindow(uint32 width, uint32 height, const char* windowTitle)
{
    int32 success = glfwInit();
    assert(success);
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    
    GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);
    
    assert(window);
    
    success = gl3wInit();
    assert(success);
    
    glfwMakeContextCurrent(window);
    
    printf("%s - %s, %s, %s\n", glGetString(GL_VERSION),
           glGetString(GL_RENDERER),
           glGetString(GL_VENDOR),
           glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    return window;
}

uint32 ggfx::createShaderProgram(uint32 type, const char* source)
{
    return glCreateShaderProgramv(type, 1, &source);
}

uint32 ggfx::createProgramPipeline(uint32 vertexProgram, uint32 fragmentProgram)
{
    char buffer[512];
    glGetProgramInfoLog(vertexProgram, 512, 0, buffer);
    printf("LOG_VERTEX: %s", buffer);
    
    char buffer2[512];
    glGetProgramInfoLog(fragmentProgram, 512, 0, buffer2);
    printf("LOG_FRAGMENT: %s", buffer);
    
    uint32 pipeline;
    glGenProgramPipelines(1, &pipeline);
    glBindProgramPipeline(pipeline);
    glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vertexProgram);
    glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fragmentProgram);
    glBindProgramPipeline(0);
    return pipeline;
}

void ggfx::draw(uint32 pipeline)
{
    glBindProgramPipeline(pipeline);
    
    //draw
    
    //glBindProgramPipeline(0);
}




