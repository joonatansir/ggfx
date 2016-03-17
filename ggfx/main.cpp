//
//  main.cpp
//  ggfx
//
//  Created by jon on 17/03/16.
//  Copyright © 2016 Joonatan Sörensen. All rights reserved.
//

/*
 
 This file is part of gl3w, hosted at https://github.com/skaslev/gl3w
 
 This is free and unencumbered software released into the public domain.
 
 Anyone is free to copy, modify, publish, use, compile, sell, or
 distribute this software, either in source code form or as a compiled
 binary, for any purpose, commercial or non-commercial, and by any
 means.
 
 In jurisdictions that recognize copyright laws, the author or authors
 of this software dedicate any and all copyright interest in the
 software to the public domain. We make this dedication for the benefit
 of the public at large and to the detriment of our heirs and
 successors. We intend this dedication to be an overt act of
 relinquishment in perpetuity of all present and future rights to this
 software under copyright law.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.
 
 */

#include <stdio.h>
#include <stdlib.h>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

int main(void)
{
    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    
    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    
    if (!window)
    {
        printf("Window creation failed!\n");
        glfwTerminate();
        return -1;
    }
    
    if (!gl3wInit())
    {
        printf("GL initialization failed!");
    }
    
    glfwMakeContextCurrent(window);
    
    printf("%s - %s, %s\n", glGetString(GL_VERSION),
                            glGetString(GL_RENDERER),
                            glGetString(GL_VENDOR));
    
    int i = 0;

    while (!glfwWindowShouldClose(window))
    {
        i++;
        
        glClearColor(0.5f*sin(i/1000.0f)+0.5f, 0.5f*cos(1.5f+i/500.0f)+0.5f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glfwSwapBuffers(window);
        
        glfwPollEvents();
        
        if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(window, 1);
        }
    }
    
    glfwTerminate();
    return 0;
}
