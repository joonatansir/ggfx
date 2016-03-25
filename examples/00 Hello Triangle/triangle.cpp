#include <stdio.h>
#include <cmath>

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "ggfx.h"
#include "util.h"
#include "debugUI.h"
#include "resources.h"

#define _CRT_SECURE_NO_WARNINGS

using namespace ggfx;

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow)
{
    uint32 width = 1280;
    uint32 height = 720;

    GLFWwindow* window = createWindow(width, height, "ggfx");
    createDebugUI(window);

    glViewport(0, 0, width, height);
    
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string error;
    bool success = tinyobj::LoadObj(shapes, materials, error, assetPath[teapot]);
    if (!success)
    {
        printf("obj loading failed! %s", error.c_str());
    }

    texture tex = createTextureFromFile(assetPath[checker_1]);
    texture tex2 = createTextureFromFile(assetPath[checker_2]); 

    const uint8* vertexSource = loadFile(assetPath[basic_vert]);
    const uint8* fragmentSource = loadFile(assetPath[basic_frag]);

    uint32 vertexProgram = createShaderProgram(GL_VERTEX_SHADER, vertexSource);
    uint32 fragmentProgram = createShaderProgram(GL_FRAGMENT_SHADER, fragmentSource);

    delete[] vertexSource;
    delete[] fragmentSource;

    uint32 pipeline = createProgramPipeline(vertexProgram, fragmentProgram);

    uint32 stride = 3 * sizeof(float32);

    buffer vertexBuffer = createBuffer(&shapes[0].mesh.positions[0], GL_ARRAY_BUFFER, shapes[0].mesh.positions.size()*sizeof(float));
    buffer indexBuffer = createBuffer(&shapes[0].mesh.indices[0], GL_ELEMENT_ARRAY_BUFFER, shapes[0].mesh.indices.size()*sizeof(unsigned int));
    
    glBindBuffer(vertexBuffer.type, vertexBuffer.id);
    glBindBuffer(indexBuffer.type, indexBuffer.id);

    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, stride, 0);
    glEnableVertexAttribArray(0);

    //color
    //glVertexAttribPointer(1, 3, GL_FLOAT, 0, stride, (void*)(3 * sizeof(float32)));
    //glEnableVertexAttribArray(1);

    //uvs
    //glVertexAttribPointer(1, 2, GL_FLOAT, 0, stride, (void*)(3 * sizeof(float32)));
    //glEnableVertexAttribArray(1);

    int32 timeLocation = glGetUniformLocation(fragmentProgram, "time");
    int32 samplerLocation = glGetUniformLocation(fragmentProgram, "sampler");
    int32 samplerLocation2 = glGetUniformLocation(fragmentProgram, "sampler2");

    int32 modelTransformLocation = glGetUniformLocation(vertexProgram, "model");
    int32 viewTransformLocation = glGetUniformLocation(vertexProgram, "view");
    int32 projectionTransformLocation = glGetUniformLocation(vertexProgram, "projection");

    
    glm::mat4 projection = glm::perspective(glm::pi<float>()/4.0f, (float)width / height, 0.1f, 100.0f);

    bool showWindow = true;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    char buffer[512];
    int32 length;
    glGetProgramInfoLog(vertexProgram, 512, &length, buffer);

    char buffer2[512];
    glGetProgramInfoLog(fragmentProgram, 512, &length, buffer2);

    glm::vec3 cameraPos;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        
        float32 time = (float32)glfwGetTime();

        newDebugUIFrame(window);

        if (showWindow)
        {
            ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Debug", &showWindow);
            ImGui::Text("Hello world!");
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.2f);
            ImGui::SliderFloat("X", &cameraPos[0], -50.0f, 50.0f);
            ImGui::SameLine();
            ImGui::SliderFloat("Y", &cameraPos[1], -50.0f, 50.0f);
            ImGui::SameLine();
            ImGui::SliderFloat("Z", &cameraPos[2], -50.0f, 50.0f);
            ImGui::End();
        }

        {
            ImGui::SetNextWindowPos(ImVec2(10, 500), ImGuiSetCond_Once);
            ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiSetCond_Once);
            ImGui::Begin("Shader Log");
            ImGui::TextWrapped(buffer);
            ImGui::TextWrapped(buffer2);
            ImGui::End();
        }

        {
            //ImGui::ShowTestWindow();
        }

        //object
        glm::mat4 world;
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
        float32 scaleAmount = 0.1f;//0.5f*(sin(time)+2.0f);
        glm::vec3 scale = glm::vec3(scaleAmount, scaleAmount, scaleAmount);
        glm::vec3 rotation = glm::vec3(0.0f, 1.0f, 0.0f);
        world = glm::translate(world, pos);
        world = glm::scale(world, scale);
        world = glm::rotate(world, time, rotation);
        
        //camera
        glm::mat4 view;
        view = glm::translate(view, cameraPos);

        glClearColor(0.5f*sin(time)+0.5f, 0.5f*cos(1.5f+time/2.0f)+0.5f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glProgramUniform1f(fragmentProgram, timeLocation, time);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex.id);
        glProgramUniform1i(fragmentProgram, samplerLocation, 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex2.id);
        glProgramUniform1i(fragmentProgram, samplerLocation2, 1);
       
        glProgramUniformMatrix4fv(vertexProgram, modelTransformLocation, 1, GL_FALSE, &world[0][0]);
        glProgramUniformMatrix4fv(vertexProgram, viewTransformLocation, 1, GL_FALSE, &view[0][0]);
        glProgramUniformMatrix4fv(vertexProgram, projectionTransformLocation, 1, GL_FALSE, &projection[0][0]);

        glBindProgramPipeline(pipeline);
        glDrawElements(GL_TRIANGLES, (GLsizei)(shapes[0].mesh.indices.size()), GL_UNSIGNED_INT, 0);
        glBindProgramPipeline(0);
        //draw(pipeline);
        
        ImGui::Render();

        glfwSwapBuffers(window);
        
        if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(window, 1);
        }
    }

    glfwTerminate();
    return 0;
}