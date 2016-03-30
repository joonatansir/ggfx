#define _CRT_SECURE_NO_WARNINGS

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
#include "DebugUI.h"
#include "resources.h"
#include "App.h"
#include "Input.h"
#include "Buffer.h"

static const float32 PI = 3.14159f;

using namespace ggfx;

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow)
{
    App app(1280, 720, "ggfx");
    DebugUI ui(app.getWindow());
    Input::Init(app.getWindow());

    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string error;
    bool success = tinyobj::LoadObj(shapes, materials, error, assetPaths[cube]);
    if (!success)
    {
        printf("obj loading failed! %s", error.c_str());
    }

    texture tex = createTextureFromFile(assetPaths[checker_1]);
    texture tex2 = createTextureFromFile(assetPaths[checker_2]); 

    const uint8* vertexSource = loadFile(assetPaths[basic_vert]);
    const uint8* fragmentSource = loadFile(assetPaths[basic_frag]);

    uint32 vertexProgram = createShaderProgram(GL_VERTEX_SHADER, vertexSource);
    uint32 fragmentProgram = createShaderProgram(GL_FRAGMENT_SHADER, fragmentSource);

    delete[] vertexSource;
    delete[] fragmentSource;

    uint32 pipeline = createProgramPipeline(vertexProgram, fragmentProgram);
    
    uint32* indices;
    uint32 vertexBufferSize;
    uint32 indexBufferSize;
    float32* dataBof = loadBOF(assetPaths[fox_bof], &indices, &vertexBufferSize, &indexBufferSize);

    GPUBuffer vertexBuffer = GPUBuffer::create(
        GL_ARRAY_BUFFER,
        vertexBufferSize,
        dataBof,
        GL_DYNAMIC_DRAW);

    GPUBuffer indexBuffer = GPUBuffer::create(
        GL_ELEMENT_ARRAY_BUFFER, 
        indexBufferSize,
        indices,
        GL_DYNAMIC_DRAW);

    vertexBuffer.bind();
    indexBuffer.bind();

    uint32 stride = 8 * sizeof(float32);
    vertexBuffer.enableVexterAttribute(0, 3, GL_FLOAT, false, stride, 0);
    vertexBuffer.enableVexterAttribute(1, 3, GL_FLOAT, false, stride, 3 * sizeof(float32));
    vertexBuffer.enableVexterAttribute(2, 2, GL_FLOAT, false, stride, 6 * sizeof(float32));

    int32 timeLocation = glGetUniformLocation(fragmentProgram, "time");
    int32 samplerLocation = glGetUniformLocation(fragmentProgram, "sampler");
    int32 samplerLocation2 = glGetUniformLocation(fragmentProgram, "sampler2");

    int32 modelTransformLocation = glGetUniformLocation(vertexProgram, "model");
    int32 viewTransformLocation = glGetUniformLocation(vertexProgram, "view");
    int32 projectionTransformLocation = glGetUniformLocation(vertexProgram, "projection");

    glm::mat4 projection = glm::perspective(PI/4.0f, (float)1280 / 720, 0.1f, 1000.0f);

    bool showWindow = true;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    char buffer[512];
    int32 length;
    glGetProgramInfoLog(vertexProgram, 512, &length, buffer);

    char buffer2[512];
    glGetProgramInfoLog(fragmentProgram, 512, &length, buffer2);

    //const uint8* stuff = loadFile(assetPaths[dragon]);

    glm::mat4 view;
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -5.0f);
    glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 1.0f);
    float32 cameraAngle = 0.0f;
    float32 cameraAngleY = 0.0f;
    float32 cameraZ = -5.f;

    bool isMiddleButtonHeld = false;

    //Object
    float32 scaleAmount = 1.0f;
    float32 rotationAmount = 0.0f;
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    
    while (!app.shouldClose())
    {
        float32 time = (float32)glfwGetTime();
        static float32 lastFrameTime = time;
        float32 dt = time - lastFrameTime;
        lastFrameTime = time;

        Input::update(app.getWindow());
        app.pollEvents();
        ui.newFrame(app.getWindow());

        //object
        glm::mat4 world;
        glm::vec3 scale = glm::vec3(scaleAmount, scaleAmount, scaleAmount);
        glm::vec3 rotation = glm::vec3(0.0f, 1.0f, 0.0f);
        world = glm::translate(world, pos);
        world = glm::scale(world, scale);
        world = glm::rotate(world, rotationAmount, rotation);
        
        cameraZ += Input::scrollOffset.y * 0.5f;
        view = glm::lookAt(cameraPos, pos, glm::vec3(0.0f, 1.0f, 0.0f));

        int32 shiftStatus = glfwGetKey(app.getWindow(), GLFW_KEY_LEFT_SHIFT);
        int32 status = glfwGetMouseButton(app.getWindow(), GLFW_MOUSE_BUTTON_MIDDLE);
        if (status == GLFW_PRESS || Input::scrollOffset.y != 0.0f)
        {
            double x, y;
            glfwGetCursorPos(app.getWindow(), &x, &y);

            static glm::vec3 lastCursorPosition;
            glm::vec3 newCursorPosition(x, y, 0);

            if (!isMiddleButtonHeld)
            {
                lastCursorPosition = newCursorPosition;
                isMiddleButtonHeld = true;
            }

            glm::vec3 offset = newCursorPosition - lastCursorPosition;

            float32 dist = cameraZ;

            cameraAngle += offset.x * dt;
            cameraAngleY += offset.y * dt;

            cameraPos.x = -sin(cameraAngle) * cos(cameraAngleY) * dist;
            cameraPos.y = -sin(cameraAngleY) * dist;
            cameraPos.z = cos(cameraAngle) * cos(cameraAngleY) * dist;

            lastCursorPosition = newCursorPosition;
        }
        else if (status == GLFW_RELEASE)
        {
            isMiddleButtonHeld = false;
        }

        if (showWindow)
        {
            ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Debug", &showWindow);
            ImGui::Text("Hello world!");
            ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.2f);
            ImGui::SliderFloat("X", &cameraPos[0], -50.0f, 50.0f);
            ImGui::SameLine();
            ImGui::SliderFloat("Y", &cameraPos[1], -50.0f, 50.0f);
            ImGui::SameLine();
            ImGui::SliderFloat("Z", &cameraPos[2], -50.0f, 50.0f);

            ImGui::Text("dt %.3f ms", dt * 1000.0f);

            ImGui::Text("scrollX: %f, scrollY: %f", Input::scrollOffset.x, Input::scrollOffset.y);
            ImGui::Text("mouse x: %f, y: %f", Input::mousePosition.x, Input::mousePosition.y);

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
            //ImGui::TextUnformatted((const char *)stuff);
            //ImGui::ShowTestWindow();
            ImGui::Begin("Object");
            ImGui::SliderFloat3("Position", &pos[0], -100.0f, 100.0f);
            ImGui::SliderFloat("Scale", &scaleAmount, 0.1f, 100.0f);
            ImGui::SliderAngle("Rotation", &rotationAmount);
            ImGui::End();
        }

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
        glDrawElements(GL_TRIANGLES, (GLsizei)(indexBufferSize/sizeof(uint32)), GL_UNSIGNED_INT, 0);

        //world = glm::scale(glm::translate(world, glm::vec3(0.0f, -2.0f, 0.0f)), glm::vec3(10.0f, 0.1f, 10.0f));
        //glProgramUniformMatrix4fv(vertexProgram, modelTransformLocation, 1, GL_FALSE, &world[0][0]);

        //glDrawElements(GL_TRIANGLES, (GLsizei)(shapes[0].mesh.indices.size()), GL_UNSIGNED_INT, 0);

        glBindProgramPipeline(0);
        //draw(pipeline);
        
        ImGui::Render();

        ui.renderDebugUI(ImGui::GetDrawData());

        app.swapBuffers();
        
        if(GLFW_PRESS == glfwGetKey(app.getWindow(), GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(app.getWindow(), 1);
        }
    }

    glfwTerminate();
    return 0;
}