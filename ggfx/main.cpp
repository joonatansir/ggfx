#include <stdio.h>
#include <stdlib.h>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "ggfx.h"

void render()
{
    static int32 i = 0;
    
    glClearColor(0.5f*sin(i/100.0f)+0.5f, 0.5f*cos(1.5f+i/500.0f)+0.5f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    ++i;
}

int main(void)
{
    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);
    
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
    
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glfwSwapBuffers(window);
        
        render();
        
        if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(window, 1);
        }
    }
    
    glfwTerminate();
    return 0;
}
