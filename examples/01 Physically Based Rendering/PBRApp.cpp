#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "PBRApp.h"
#include "Log.h"
#include "resources.h"
#include "Texture.h"
#include "ShaderPipeline.h"
#include "util.h"
#include "VertexBuffer.h"
#include "GPUBuffer.h"
#include "Input.h"
#include "Assets.h"
#include "ShaderEditing.h"

//TODO: remove this
#include "GLFWWindow.h"

using namespace ggfx;

PBRApp::PBRApp(uint32 width, uint32 height, const char* title) :
    App(width, height, title),
    ui(window)
{
}

PBRApp::~PBRApp()
{
}

static const float PI = 3.14159f;

static glm::vec2 lerp(glm::vec2& a, glm::vec2& b, float t)
{
    return a + (b - a)*t;
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

static ShaderPipeline pipeline;
static ShaderPipeline pipeline2;

static uint32 indexBufferSize;

static int32 timeLocation_vs;
static int32 timeLocation;
static int32 samplerLocation;
static int32 samplerLocation2;
static int32 cubemapLocation;
static int32 cubemapSamplerLocation;

static int32 modelTransformLocation;
static int32 viewTransformLocation;
static int32 projectionTransformLocation;
static int32 viewCubemap;
static int32 projectionLoc;

//Object
static float scaleAmount = 1.0f;
static float rotationAmount = 0.0f;
static glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);

//Camera
static glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 20.0f);
static float angle = 0.0f;
static glm::quat rot(cos(angle / 2.0f), sin(angle / 2.0f)*glm::vec3(0.0f, 1.0f, 0.0f));

static glm::mat4 projection = glm::perspective(PI / 4.0f, (float)1280 / 720, 0.1f, 1000.0f);

static glm::vec2 lastCursorPosition;

void PBRApp::update(float dt)
{
    CHECK_FOR_SHADER_UPDATE(pipeline.vertexShader);
    CHECK_FOR_SHADER_UPDATE(pipeline.fragmentShader);

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

    ui.update(window);

    const int32 frames = 100;
    static float averageFrameRate = 0.0f;
    static float rollingFrametimeCounter[frames] = { 0.0f };
    static int32 currentFrame = 0;
    
    averageFrameRate += dt - rollingFrametimeCounter[currentFrame];
    rollingFrametimeCounter[currentFrame] = dt;
    currentFrame = ++currentFrame % frames;
    
    Log::warning("%f\n", averageFrameRate);

    {
        ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f));
        ImGui::Begin("Stats", 0, ImVec2(0, 0), 0.3f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
        ImGui::Text("dt %.3f ms, %.1f FPS", dt * 1000.0f, 1.0f / (averageFrameRate / frames));
        ImGui::Text("mouse x: %f, y: %f", Input::mousePosition.x, Input::mousePosition.y);
        ImGui::End();
    }

    {
        ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Debug");
        ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.2f);
        ImGui::SliderFloat("X", &cameraPos[0], -50.0f, 50.0f);
        ImGui::SameLine();
        ImGui::SliderFloat("Y", &cameraPos[1], -50.0f, 50.0f);
        ImGui::SameLine();
        ImGui::SliderFloat("Z", &cameraPos[2], -50.0f, 50.0f);
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

    glProgramUniform1f(pipeline.vertexShader.id, timeLocation_vs, (float)getTime());
    glProgramUniform1f(pipeline.fragmentShader.id, timeLocation, (float)getTime());
    glProgramUniform1i(pipeline.fragmentShader.id, samplerLocation, 1);
    glProgramUniform1i(pipeline.fragmentShader.id, samplerLocation2, 2);
    glProgramUniform1i(pipeline.fragmentShader.id, cubemapSamplerLocation, 3);
    glProgramUniform1i(pipeline2.fragmentShader.id, cubemapLocation, 3);

    glProgramUniformMatrix4fv(pipeline2.vertexShader.id, viewCubemap, 1, GL_FALSE, &view[0][0]);
    glProgramUniformMatrix4fv(pipeline2.vertexShader.id, projectionLoc, 1, GL_FALSE, &projection[0][0]);

    glProgramUniformMatrix4fv(pipeline.vertexShader.id, modelTransformLocation, 1, GL_FALSE, &world[0][0]);
    glProgramUniformMatrix4fv(pipeline.vertexShader.id, viewTransformLocation, 1, GL_FALSE, &view[0][0]);
    glProgramUniformMatrix4fv(pipeline.vertexShader.id, projectionTransformLocation, 1, GL_FALSE, &projection[0][0]);

    glBindProgramPipeline(pipeline2.id);

    //debug only
    pipeline2.useProgramStage(pipeline2.vertexShader.id, GL_VERTEX_SHADER_BIT);
    pipeline2.useProgramStage(pipeline2.fragmentShader.id, GL_FRAGMENT_SHADER_BIT);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindProgramPipeline(pipeline.id);

    pipeline.useProgramStage(pipeline.vertexShader.id, GL_VERTEX_SHADER_BIT);
    pipeline.useProgramStage(pipeline.fragmentShader.id, GL_FRAGMENT_SHADER_BIT);

    glDrawElementsInstanced(
        GL_TRIANGLES,
        (GLsizei)(indexBufferSize / sizeof(uint32)),
        GL_UNSIGNED_INT,
        0,
        1);

    ImGui::Render();
    ui.render(ImGui::GetDrawData());

    if (GLFW_PRESS == glfwGetKey(window->handle->ptr, GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(window->handle->ptr, 1);
    }
}

void PBRApp::init()
{
    Assets::scanAssetDirectory("assets/");

    //TODO: Remove this from here
    uint32 vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const std::string filenames[] = {
        Assets::getPath("sky/sky_right.bmp"),
        Assets::getPath("sky/sky_left.bmp"),
        Assets::getPath("sky/sky_top.bmp"),
        Assets::getPath("sky/sky_bottom.bmp"),
        Assets::getPath("sky/sky_front.bmp"),
        Assets::getPath("sky/sky_back.bmp"),
    };

    Texture vivi_color = Texture::create2DFromFile(Assets::getPath("vivi_color.psd"));
    Texture cubemap = Texture::createCubemapFromFile(filenames, { GL_RGBA, false });
    Texture texture = Texture::create2DFromFile(Assets::getPath("checker2.png"));
    Texture texture2 = Texture::create2DFromFile(Assets::getPath("checker.png"));

    texture2.bind(2);
    vivi_color.bind(1);
    cubemap.bind(3);

    pipeline = ShaderPipeline::createPipelineFromFile("basic");
    pipeline2 = ShaderPipeline::createPipelineFromFile("cubemap");

    uint32* indices;
    uint32 vertexBufferSize;
    float* dataBof = loadBOF(Assets::getPath("dr.bof"), &indices, &vertexBufferSize, &indexBufferSize);

    VertexBuffer vertexBuffer(vertexBufferSize, dataBof, GL_STATIC_DRAW);
    GPUBuffer indexBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, indices, GL_STATIC_DRAW);

    vertexBuffer.bind();
    indexBuffer.bind();

    uint32 stride = 8 * sizeof(float);
    vertexBuffer.enableVexterAttribute(0, 3, GL_FLOAT, false, stride, 0);
    vertexBuffer.enableVexterAttribute(1, 3, GL_FLOAT, false, stride, (void *)(3 * sizeof(float)));
    vertexBuffer.enableVexterAttribute(2, 2, GL_FLOAT, false, stride, (void *)(6 * sizeof(float)));

    pipeline.vertexShader.getUniformLocation(&timeLocation_vs, "time");
    pipeline.fragmentShader.getUniformLocation(&timeLocation, "time");
    pipeline.fragmentShader.getUniformLocation(&samplerLocation, "sampler");
    pipeline.fragmentShader.getUniformLocation(&samplerLocation2, "sampler2");
    pipeline2.fragmentShader.getUniformLocation(&cubemapLocation, "cubemap");
    pipeline.fragmentShader.getUniformLocation(&cubemapSamplerLocation, "cubemapSampler");

    pipeline.vertexShader.getUniformLocation(&modelTransformLocation, "model");
    pipeline.vertexShader.getUniformLocation(&viewTransformLocation, "view");
    pipeline.vertexShader.getUniformLocation(&projectionTransformLocation, "projection");
    pipeline2.vertexShader.getUniformLocation(&viewCubemap, "view");
    pipeline2.vertexShader.getUniformLocation(&projectionLoc, "projection");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}
