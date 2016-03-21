#include <stdio.h>
#include <cmath>

#include "GL/gl3w.h"

#include "ggfx.h"
#include "util.h"

using namespace ggfx;

int main(void)
{
    GLFWwindow* window = createWindow(640, 480, "ggfx");
    
    int32 i = 0;
    
    float32 points[] =
    {
        -0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
    };
    
    float32 colors[] =
    {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
    };
    
    uint32 vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    
    const char* vertexSource = loadFile("../../examples/Hello Triangle/assets/basic.vert");
    const char* fragmentSource = loadFile("../../examples/Hello Triangle/assets/basic.frag");
    
    uint32 vertexProgram = createShaderProgram(GL_VERTEX_SHADER, vertexSource);
    uint32 fragmentProgram = createShaderProgram(GL_FRAGMENT_SHADER, fragmentSource);
    
    uint32 pipeline = createProgramPipeline(vertexProgram, fragmentProgram);
    
    uint32 vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 3*sizeof(float32), 0);
    glEnableVertexAttribArray(0);
    
    while (!glfwWindowShouldClose(window))
    {
        ++i;
        
        glClearColor(0.5f*sin(i/100.0f)+0.5f, 0.5f*cos(1.5f+i/500.0f)+0.5f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        draw(pipeline);
        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glfwPollEvents();
        glfwSwapBuffers(window);
        
        if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(window, 1);
        }
    }
    
    glfwTerminate();
    return 0;
}