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

static glm::vec2 lerp(glm::vec2& a, glm::vec2& b, float t)
{
    return a + (b-a)*t;
}

static void fpsCamera(glm::quat& rotation, glm::vec3& pos, glm::vec2& delta, float movementSpeed, float dt, int32 key_w, int32 key_a, int32 key_s, int32 key_d, int32 key_q, int32 key_e)
{
    glm::vec3 right = rotation * glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    glm::quat xRot(cos(-delta.x / 2.0f), sin(-delta.x / 2.0f)*up);
    glm::quat yRot(cos(-delta.y / 2.0f), sin(-delta.y / 2.0f)*right);

    rotation = xRot * yRot * rotation;

    glm::vec3 updown(0.0f, key_e - key_q, 0.0f);
    glm::vec3 rightleft(rotation * glm::vec3(key_d - key_a, 0.0f, 0.0f));
    glm::vec3 frontback(rotation * glm::vec3(0.0f, 0.0f, -(key_w - key_s)));

    static glm::vec3 velocity;
    velocity += (updown + rightleft + frontback) * movementSpeed;
    pos += velocity * dt;
    velocity *= 0.8f;
}

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

    Texture cubemap = Texture::createCubeFromFile(filenames, GL_RGBA, false);
    cubemap.bind(GL_TEXTURE2);

    /*Texture saint_cubemap = Texture::createCubeFromFile(assetPaths[saint_posx], 
                                                        assetPaths[saint_negx], 
                                                        assetPaths[saint_posy], 
                                                        assetPaths[saint_negy], 
                                                        assetPaths[saint_posz], 
                                                        assetPaths[saint_negz], GL_RGBA, false);
    saint_cubemap.bind(GL_TEXTURE2);*/

    Texture texture = Texture::create2DFromFile(assetPaths[checker_1]);
    Texture texture2 = Texture::create2DFromFile(assetPaths[checker_2]);
    texture2.bind(GL_TEXTURE1);
    texture.bind(GL_TEXTURE0);

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
    float32* dataBof = loadBOF(assetPaths[sphere_bof], &indices, &vertexBufferSize, &indexBufferSize);

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
    int32 cubemapSamplerLocation = pipeline.fs.getUniformLocation("cubemapSampler");

    int32 modelTransformLocation = pipeline.vs.getUniformLocation("model");
    int32 viewTransformLocation = pipeline.vs.getUniformLocation("view");
    int32 projectionTransformLocation = pipeline.vs.getUniformLocation("projection");
    int32 viewCubemap = pipeline2.vs.getUniformLocation("view");
    int32 projectionLoc = pipeline2.vs.getUniformLocation("projection");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LEQUAL);
    
    //Object
    float32 scaleAmount = 1.0f;
    float32 rotationAmount = 0.0f;
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 20.0f);
    float angle = 0.0f;
    glm::quat rot(cos(angle/2.0f), sin(angle/2.0f)*glm::vec3(0.0f, 1.0f, 0.0f));
    //glm::vec3 right(1.0f, 0.0f, 0.0f);

    glm::mat4 projection = glm::perspective(PI/4.0f, (float)1280 / 720, 0.1f, 1000.0f);

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
        
        int32 key_w = (glfwGetKey(window->handle->ptr, GLFW_KEY_W) == GLFW_PRESS);
        int32 key_a = (glfwGetKey(window->handle->ptr, GLFW_KEY_A) == GLFW_PRESS);
        int32 key_s = (glfwGetKey(window->handle->ptr, GLFW_KEY_S) == GLFW_PRESS);
        int32 key_d = (glfwGetKey(window->handle->ptr, GLFW_KEY_D) == GLFW_PRESS);
        int32 key_q = (glfwGetKey(window->handle->ptr, GLFW_KEY_Q) == GLFW_PRESS);
        int32 key_e = (glfwGetKey(window->handle->ptr, GLFW_KEY_E) == GLFW_PRESS);

        glm::vec2 delta(0);
        int32 status = glfwGetMouseButton(window->handle->ptr, GLFW_MOUSE_BUTTON_RIGHT);
        if (status == GLFW_PRESS)
        {
            glm::vec2 newCursorPosition(Input::mousePosition.x, Input::mousePosition.y);
            delta = lerp(lastCursorPosition, newCursorPosition, 0.1f * dt) - lastCursorPosition;
        }

        float movementSpeed = dt * 400.0f;

        if ((glfwGetKey(window->handle->ptr, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
        {
            movementSpeed *= 0.1f;
        }

        lastCursorPosition = glm::vec2(Input::mousePosition.x, Input::mousePosition.y);

        fpsCamera(rot, cameraPos, delta, movementSpeed, dt, key_w, key_a, key_s, key_d, key_q, key_e);
        
        glm::mat4 view;
        view = glm::translate(view, cameraPos);
        view = view * glm::mat4_cast(rot);
        view = glm::inverse(view);

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
                pipeline2.recompile();

                timeLocation_vs = pipeline.vs.getUniformLocation("time");
                timeLocation = pipeline.fs.getUniformLocation("time");
                samplerLocation = pipeline.fs.getUniformLocation("sampler");
                samplerLocation2 = pipeline.fs.getUniformLocation("sampler2");
                cubemapLocation = pipeline2.fs.getUniformLocation("cubemap");
                cubemapSamplerLocation = pipeline.fs.getUniformLocation("cubemapSampler");

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
        glProgramUniform1i(pipeline.fs.id, cubemapSamplerLocation, 2);
        glProgramUniform1i(pipeline2.fs.id, cubemapLocation, 2);

        glProgramUniformMatrix4fv(pipeline2.vs.id, viewCubemap, 1, GL_FALSE, &view[0][0]);
        glProgramUniformMatrix4fv(pipeline2.vs.id, projectionLoc, 1, GL_FALSE, &projection[0][0]);
        
        glProgramUniformMatrix4fv(pipeline.vs.id, modelTransformLocation, 1, GL_FALSE, &world[0][0]);
        glProgramUniformMatrix4fv(pipeline.vs.id, viewTransformLocation, 1, GL_FALSE, &view[0][0]);
        glProgramUniformMatrix4fv(pipeline.vs.id, projectionTransformLocation, 1, GL_FALSE, &projection[0][0]);

        glBindProgramPipeline(pipeline2.id);

        pipeline2.useProgramStage(pipeline2.vs.id, GL_VERTEX_SHADER_BIT);
        pipeline2.useProgramStage(pipeline2.fs.id, GL_FRAGMENT_SHADER_BIT);

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
            100);

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