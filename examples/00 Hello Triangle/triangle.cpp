#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <cmath>

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>

#include "ggfx.h"
#include "util.h"
#include "DebugUI.h"
#include "resources.h"
#include "App.h"
#include "Input.h"
#include "GPUBuffer.h"
#include "Log.h"
#include "Texture.h"
#include "Window.h"
#include "GLFWWindow.h"

using namespace ggfx;

static const float32 PI = 3.14159f;

int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    uint32 width = 1600;
    uint32 height = 900;

    Window* window = new Window(width, height, "ggfx");
    App app(window);

    //TODO: Remove this from here
    uint32 vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glViewport(0, 0, width, height);

    const char* filenames[] = {
        assetPaths[cubemap_posx],
        assetPaths[cubemap_negx],
        assetPaths[cubemap_posy],
        assetPaths[cubemap_negy],
        assetPaths[cubemap_posz],
        assetPaths[cubemap_negz],
    };

    /*Texture cubemap = Texture::createCubeFromFile(filenames);
    cubemap.bind(GL_TEXTURE2);*/

    Texture texture = Texture::create2DFromFile(assetPaths[checker_1]);
    Texture texture2 = Texture::create2DFromFile(assetPaths[checker_2]);
    texture2.bind(GL_TEXTURE1);
    texture.bind(GL_TEXTURE0);

    uint32 cubemapVertexProgram = createShaderProgramFromFile(assetPaths[cubemap_vert_shader], GL_VERTEX_SHADER);
    uint32 cubemapFragmentProgram = createShaderProgramFromFile(assetPaths[cubemap_frag_shader], GL_FRAGMENT_SHADER);

    uint32 vertexProgram = createShaderProgramFromFile(assetPaths[basic_vert], GL_VERTEX_SHADER);
    uint32 fragmentProgram = createShaderProgramFromFile(assetPaths[basic_frag], GL_FRAGMENT_SHADER);

    uint32 pipeline = createProgramPipeline(vertexProgram, fragmentProgram);
    uint32 pipeline2 = createProgramPipeline(cubemapVertexProgram, cubemapFragmentProgram);
    
    uint32* indices;
    uint32 vertexBufferSize;
    uint32 indexBufferSize;
    float32* dataBof = loadBOF(assetPaths[sphere_bof], &indices, &vertexBufferSize, &indexBufferSize);

    //uint32* indices2;
    //uint32 vbs;
    //uint32 ibs;
    //uint32 uvs;
    //uint32 ns;
    ////float32* dataBob = loadBinaryOBJ(assetPaths[sphere_bob], &indices2, vbs, uvs, ns, ibs);

    GPUBuffer vertexBuffer = GPUBuffer::create(
        GL_ARRAY_BUFFER,
        vertexBufferSize,
        dataBof,
        GL_STATIC_DRAW);

    GPUBuffer indexBuffer = GPUBuffer::create(
        GL_ELEMENT_ARRAY_BUFFER, 
        indexBufferSize,
        indices,
        GL_DYNAMIC_DRAW);

    vertexBuffer.bind();
    indexBuffer.bind();

    uint32 size = vertexBufferSize / sizeof(float32);
    for (uint32 i = 0; i < size; i+=8)
    {
        Log::print("%d:\n", i/8);
        Log::print("vp: %f, %f, %f\n", dataBof[i], dataBof[i + 1], dataBof[i + 2]);
        Log::print("vn: %f, %f, %f\n", dataBof[i + 3], dataBof[i + 4], dataBof[i + 5]);
        Log::print("vt: %f, %f\n", dataBof[i + 6], dataBof[i + 7]);
    }

    uint32 stride = 8 * sizeof(float32);
    vertexBuffer.enableVexterAttribute(0, 3, GL_FLOAT, false, stride, 0);
    vertexBuffer.enableVexterAttribute(1, 3, GL_FLOAT, false, stride, 3 * sizeof(float32));
    vertexBuffer.enableVexterAttribute(2, 2, GL_FLOAT, false, stride, 6 * sizeof(float32));

    int32 timeLocation = glGetUniformLocation(fragmentProgram, "time");
    int32 samplerLocation = glGetUniformLocation(fragmentProgram, "sampler");
    int32 samplerLocation2 = glGetUniformLocation(fragmentProgram, "sampler2");
    int32 cubemapLocation = glGetUniformLocation(cubemapFragmentProgram, "cubemap");

    int32 modelTransformLocation = glGetUniformLocation(vertexProgram, "model");
    int32 viewTransformLocation = glGetUniformLocation(vertexProgram, "view");
    int32 projectionTransformLocation = glGetUniformLocation(vertexProgram, "projection");
    int32 viewCubemap = glGetUniformLocation(cubemapVertexProgram, "view");

    glm::mat4 projection = glm::perspective(PI/4.0f, (float)1280 / 720, 0.1f, 1000.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LEQUAL);

    char buffer[512];
    int32 length;
    glGetProgramInfoLog(vertexProgram, 512, &length, buffer);

    char buffer2[512];
    glGetProgramInfoLog(fragmentProgram, 512, &length, buffer2);

    char buffer3[512];
    glGetProgramInfoLog(cubemapVertexProgram, 512, &length, buffer3);

    char buffer4[512];
    glGetProgramInfoLog(cubemapFragmentProgram, 512, &length, buffer4);

    Log::print(buffer);
    Log::print(buffer2);
    Log::print(buffer3);
    Log::print(buffer4);
    
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
    
    Log::print("HELLO %d, %d", 999, 123);

    while (!window->shouldClose())
    {
        float32 time = (float32)glfwGetTime();
        static float32 lastFrameTime = time;
        float32 dt = time - lastFrameTime;
        lastFrameTime = time;

        app.update();

        //object
        glm::mat4 world;
        glm::vec3 scale = glm::vec3(scaleAmount, scaleAmount, scaleAmount);
        glm::vec3 rotation = glm::vec3(0.0f, 1.0f, 0.0f);
        world = glm::translate(world, pos);
        world = glm::scale(world, scale);
        world = glm::rotate(world, rotationAmount, rotation);
        
        cameraZ += Input::scrollOffset.y * 0.5f;
        view = glm::lookAt(cameraPos, pos, glm::vec3(0.0f, 1.0f, 0.0f));

        int32 shiftStatus = glfwGetKey(window->handle->ptr, GLFW_KEY_LEFT_SHIFT);
        int32 status = glfwGetMouseButton(window->handle->ptr, GLFW_MOUSE_BUTTON_MIDDLE);
        if (status == GLFW_PRESS || Input::scrollOffset.y != 0.0f)
        {
            double x, y;
            glfwGetCursorPos(window->handle->ptr, &x, &y);

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

        {
            ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Debug");
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
            int32 maxUniformBufferSize;
            int32 majorVersion;
            glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
            glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBufferSize);
            ImGui::Text("Max uniform buffer size: %d bytes", maxUniformBufferSize);
            ImGui::Text("Major version: %d", majorVersion);
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

        //glClearColor(0.5f*sin(time)+0.5f, 0.5f*cos(1.5f+time/2.0f)+0.5f, 0.2f, 1.0f);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glProgramUniform1f(fragmentProgram, timeLocation, time);
        glProgramUniform1i(fragmentProgram, samplerLocation, 0);
        glProgramUniform1i(fragmentProgram, samplerLocation2, 1);
        glProgramUniform1i(cubemapFragmentProgram, cubemapLocation, 2);

        glProgramUniformMatrix4fv(vertexProgram, modelTransformLocation, 1, GL_FALSE, &world[0][0]);
        glProgramUniformMatrix4fv(vertexProgram, viewTransformLocation, 1, GL_FALSE, &view[0][0]);
        glProgramUniformMatrix4fv(vertexProgram, projectionTransformLocation, 1, GL_FALSE, &projection[0][0]);
        glProgramUniformMatrix4fv(cubemapVertexProgram, viewCubemap, 1, GL_FALSE, &view[0][0]);

        glBindProgramPipeline(pipeline2);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);

        glBindProgramPipeline(pipeline);

        glDrawElementsInstanced(
            GL_TRIANGLES, 
            (GLsizei)(indexBufferSize / sizeof(uint32)),
            GL_UNSIGNED_INT,
            0, 
            1);

        //glBindProgramPipeline(0);
        
        ImGui::Render();

        app.render();
        
        if(GLFW_PRESS == glfwGetKey(window->handle->ptr, GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(window->handle->ptr, 1);
        }
    }

    delete window;
    return 0;
}