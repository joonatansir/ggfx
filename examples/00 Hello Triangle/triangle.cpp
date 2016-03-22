#include <stdio.h>
#include <cmath>

#include "GL/gl3w.h"
#include "glm/glm.hpp"

#include "ggfx.h"
#include "util.h"

using namespace ggfx;

int main(void)
{
    GLFWwindow* window = createWindow(800, 800, "ggfx");
    
    float32 points[] =
    {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 0.2f,   0.0f, 1.0f,
        0.5f, 0.5f, 0.0f,   1.0f, 0.5f, 0.2f,   1.0f, 1.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 0.1f, 1.0f,   1.0f, 0.0f,
    };
    
    uint32 indices[] =
    {
        0, 2, 1,
        0, 3, 2,
    };
    
    buffer vertexBuffer = createBuffer(points, GL_ARRAY_BUFFER, sizeof(points));
    buffer indexBuffer = createBuffer(indices, GL_ELEMENT_ARRAY_BUFFER, sizeof(indices));
    
    texture tex = createTextureFromFile("../../examples/00 Hello Triangle/assets/checker.png");
    texture tex2 = createTextureFromFile("../../examples/00 Hello Triangle/assets/checker2.png");
    
    const uint8* vertexSource = loadFile("../../examples/00 Hello Triangle/assets/basic.vert");
    const uint8* fragmentSource = loadFile("../../examples/00 Hello Triangle/assets/basic.frag");
        
    uint32 vertexProgram = createShaderProgram(GL_VERTEX_SHADER, vertexSource);
    uint32 fragmentProgram = createShaderProgram(GL_FRAGMENT_SHADER, fragmentSource);
    
    delete[] vertexSource;
    delete[] fragmentSource;
    
    uint32 pipeline = createProgramPipeline(vertexProgram, fragmentProgram);
    
    uint32 stride = 8 * sizeof(float32);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, stride, 0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, 0, stride, (void*)(3*sizeof(float32)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, 0, stride, (void*)(6*sizeof(float32)));
    glEnableVertexAttribArray(2);
    
    int32 timeLocation = glGetUniformLocation(fragmentProgram, "time");
    int32 samplerLocation = glGetUniformLocation(fragmentProgram, "sampler");
    int32 samplerLocation2 = glGetUniformLocation(fragmentProgram, "sampler2");
    
    while (!glfwWindowShouldClose(window))
    {
        float32 time = (float32)glfwGetTime();
        
        glClearColor(0.5f*sin(time)+0.5f, 0.5f*cos(1.5f+time/2.0f)+0.5f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glProgramUniform1f(fragmentProgram, timeLocation, time);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex.id);
        glProgramUniform1i(fragmentProgram, samplerLocation, 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex2.id);
        glProgramUniform1i(fragmentProgram, samplerLocation2, 1);
        
        draw(pipeline);
        
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