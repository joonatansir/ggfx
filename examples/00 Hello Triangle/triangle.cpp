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
#include "ShaderPipeline.h"

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

    Texture cubemap = Texture::createCubeFromFile(filenames);
    cubemap.bind(GL_TEXTURE2);

    Texture texture = Texture::create2DFromFile(assetPaths[checker_1]);
    Texture texture2 = Texture::create2DFromFile(assetPaths[checker_2]);
    texture2.bind(GL_TEXTURE1);
    texture.bind(GL_TEXTURE0);

    //uint32 cubemapVertexProgram = createShaderProgramFromFile(assetPaths[cubemap_vert_shader], GL_VERTEX_SHADER);
    //uint32 cubemapFragmentProgram = createShaderProgramFromFile(assetPaths[cubemap_frag_shader], GL_FRAGMENT_SHADER);

    //uint32 vertexProgram = createShaderProgramFromFile(assetPaths[basic_vert], GL_VERTEX_SHADER);
    //uint32 fragmentProgram = createShaderProgramFromFile(assetPaths[basic_frag], GL_FRAGMENT_SHADER);

    //uint32 pipeline = createProgramPipeline(vertexProgram, fragmentProgram);
    //uint32 pipeline2 = createProgramPipeline(cubemapVertexProgram, cubemapFragmentProgram);

    Shader meshShaders[2] = {0};
    Shader cubeShaders[2] = {0};
    
    cubeShaders[0] = ShaderPipeline::createProgramFromFile(assetPaths[cubemap_vert_shader], GL_VERTEX_SHADER);
    cubeShaders[1] = ShaderPipeline::createProgramFromFile(assetPaths[cubemap_frag_shader], GL_FRAGMENT_SHADER);

    meshShaders[0] = ShaderPipeline::createProgramFromFile(assetPaths[basic_vert], GL_VERTEX_SHADER);
    meshShaders[1] = ShaderPipeline::createProgramFromFile(assetPaths[basic_frag], GL_FRAGMENT_SHADER);

    ShaderPipeline pipeline = ShaderPipeline::createPipeline(meshShaders[0], meshShaders[1]);
    ShaderPipeline pipeline2 = ShaderPipeline::createPipeline(cubeShaders[0], cubeShaders[1]);
    
    uint32* indices;
    uint32 vertexBufferSize;
    uint32 indexBufferSize;
    float32* dataBof = loadBOF(assetPaths[fox_bof], &indices, &vertexBufferSize, &indexBufferSize);

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

    uint32 stride = 8 * sizeof(float32);
    vertexBuffer.enableVexterAttribute(0, 3, GL_FLOAT, false, stride, 0);
    vertexBuffer.enableVexterAttribute(1, 3, GL_FLOAT, false, stride, 3 * sizeof(float32));
    vertexBuffer.enableVexterAttribute(2, 2, GL_FLOAT, false, stride, 6 * sizeof(float32));

    int32 timeLocation_vs = pipeline.vs.getUniformLocation("time");
    int32 timeLocation = pipeline.fs.getUniformLocation("time");
    int32 samplerLocation = pipeline.fs.getUniformLocation("sampler");
    int32 samplerLocation2 = pipeline.fs.getUniformLocation("sampler2");
    int32 cubemapLocation = pipeline2.fs.getUniformLocation("cubemap");

    int32 modelTransformLocation = pipeline.vs.getUniformLocation("model");
    int32 viewTransformLocation = pipeline.vs.getUniformLocation("view");
    int32 projectionTransformLocation = pipeline.vs.getUniformLocation("projection");
    int32 viewCubemap = pipeline2.vs.getUniformLocation("view");
    int32 projectionLoc = pipeline2.vs.getUniformLocation("projection");

    glm::mat4 projection = glm::perspective(PI/4.0f, (float)1280 / 720, 0.1f, 1000.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LEQUAL);

    //char buffer[512] = {};
    //int32 length;
    //glGetProgramInfoLog(vertexProgram, 512, &length, buffer);

    //char buffer2[512];
    //glGetProgramInfoLog(fragmentProgram, 512, &length, buffer2);

    //char buffer3[512];
    //glGetProgramInfoLog(cubemapVertexProgram, 512, &length, buffer3);

    //char buffer4[512];
    //glGetProgramInfoLog(cubemapFragmentProgram, 512, &length, buffer4);

    //Log::print(buffer);
    //Log::print(buffer2);
    //Log::print(buffer3);
    //Log::print(buffer4);
    
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
    
    glm::quat rot;
    //glm::vec3 right(1.0f, 0.0f, 0.0f);

    glm::vec2 lastCursorPosition;

    while (!window->shouldClose())
    {
        float32 time = (float32)glfwGetTime();
        static float32 lastFrameTime = time;
        float32 dt = time - lastFrameTime;
        lastFrameTime = time;

        //object
        glm::mat4 world;
        glm::vec3 scale = glm::vec3(scaleAmount, scaleAmount, scaleAmount);
        glm::vec3 rotation = glm::vec3(0.0f, 1.0f, 0.0f);
        world = glm::translate(world, pos);
        world = glm::scale(world, scale);
        world = glm::rotate(world, rotationAmount, rotation);
        
        glm::mat4 view;
        glm::vec3 right(1.0f, 0.0f, 0.0f);
        glm::vec3 up(0.0f, 1.0f, 0.0f);
        glm::vec3 forward(0.0f, 0.0f, -1.0f);
        
        int32 status = glfwGetMouseButton(window->handle->ptr, GLFW_MOUSE_BUTTON_LEFT);
        if (status == GLFW_PRESS)
        {
            glm::vec2 newCursorPosition(Input::mousePosition.x, Input::mousePosition.y);

            glm::vec2 delta = (newCursorPosition - lastCursorPosition) * dt;
            glm::quat xRot(cos(delta.x / 2.0f), sin(delta.x / 2.0f)*up);

            right = xRot * right;

            glm::quat yRot(cos(delta.y / 2.0f), sin(delta.y / 2.0f)*right);
            
            rot = rot * xRot;
            rot = rot * yRot;

            //glm::translate(view, glm::vec3(1.0f, 3.0f, 1.0f));
        }

        view = glm::mat4_cast(rot);

        right = rot * right;
        up = rot * up;
        forward = rot * forward;

        //Log::print("%f, %f, %f\n", forward.x, forward.y, forward.z);
        
        int32 w = glfwGetKey(window->handle->ptr, GLFW_KEY_W);
        if (w == GLFW_PRESS)
        {
            cameraPos -= forward * dt * 5.0f;
        }
        else if (glfwGetKey(window->handle->ptr, GLFW_KEY_S))
        {
            cameraPos += forward * dt * 5.0f;
        }

        view = glm::translate(view, cameraPos);

        lastCursorPosition = glm::vec2(Input::mousePosition.x, Input::mousePosition.y);

        /*cameraZ += Input::scrollOffset.y * 0.5f;
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
        }*/

        app.update();

        {
            ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Debug");
            ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.2f);
            ImGui::SliderFloat("X", &cameraPos[0], -50.0f, 50.0f);
            ImGui::SameLine();
            ImGui::SliderFloat("Y", &cameraPos[1], -50.0f, 50.0f);
            ImGui::SameLine();
            ImGui::SliderFloat("Z", &cameraPos[2], -50.0f, 50.0f);

            ImGui::Text("dt %.3f ms", dt * 1000.0f);

            ImGui::Text("scrollX: %f, scrollY: %f", Input::scrollOffset.x, Input::scrollOffset.y);
            ImGui::Text("mouse x: %f, y: %f", Input::mousePosition.x, Input::mousePosition.y);

            if (ImGui::Button("Recompile shaders"))
            {
                pipeline.recompile();

                timeLocation_vs = pipeline.vs.getUniformLocation("time");
                timeLocation = pipeline.fs.getUniformLocation("time");
                samplerLocation = pipeline.fs.getUniformLocation("sampler");
                samplerLocation2 = pipeline.fs.getUniformLocation("sampler2");
                cubemapLocation = pipeline2.fs.getUniformLocation("cubemap");

                modelTransformLocation = pipeline.vs.getUniformLocation("model");
                viewTransformLocation = pipeline.vs.getUniformLocation("view");
                projectionTransformLocation = pipeline.vs.getUniformLocation("projection");
                viewCubemap = pipeline2.vs.getUniformLocation("view");
                projectionLoc = pipeline2.vs.getUniformLocation("projection");
            }

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

        glProgramUniform1f(pipeline.vs.id, timeLocation_vs, time);
        glProgramUniform1f(pipeline.fs.id, timeLocation, time);
        glProgramUniform1i(pipeline.fs.id, samplerLocation, 0);
        glProgramUniform1i(pipeline.fs.id, samplerLocation2, 1);
        glProgramUniform1i(pipeline2.fs.id, cubemapLocation, 2);

        glProgramUniformMatrix4fv(pipeline.vs.id, modelTransformLocation, 1, GL_FALSE, &world[0][0]);
        glProgramUniformMatrix4fv(pipeline.vs.id, viewTransformLocation, 1, GL_FALSE, &view[0][0]);
        glProgramUniformMatrix4fv(pipeline.vs.id, projectionTransformLocation, 1, GL_FALSE, &projection[0][0]);
        glProgramUniformMatrix4fv(pipeline2.vs.id, viewCubemap, 1, GL_FALSE, &view[0][0]);
        glProgramUniformMatrix4fv(pipeline2.vs.id, projectionLoc, 1, GL_FALSE, &projection[0][0]);

        glBindProgramPipeline(pipeline2.id);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);

        glBindProgramPipeline(pipeline.id);

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