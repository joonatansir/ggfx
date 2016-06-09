#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "PBRApp.h"
#include "Log.h"
#include "Texture.h"
#include "ShaderPipeline.h"
#include "util.h"
#include "GPUBuffer.h"
#include "Input.h"
#include "Assets.h"
#include "ShaderEditing.h"
#include "Shader.h"
#include "GPUTimer.h"

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

static Shader basicVert;
static Shader basicFrag;
static Shader CubemapVert;
static Shader CubemapFrag;
static Shader voxelGeom;
static Shader voxelFrag;
static Shader voxelVert;
static Shader visualizeVoxelVert;
static Shader visualizeVoxelFrag;

static uint32 indexBufferSize;
static uint32 cubeVertexBufferSize;
static uint32 cubeIndexBufferSize;
static GPUBuffer cubeVertexBuffer(BufferTarget::Array);
static GPUBuffer cubeIndexBuffer(BufferTarget::Index);
static GPUBuffer vertexBuffer(BufferTarget::Array);
static GPUBuffer indexBuffer(BufferTarget::Index);

uint32 vaos[2];

static glm::ivec2 windowSize;

//Object
static float scaleAmount = 1.0f;
static float rotationAmount = 0.0f;
static glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);

//Camera
static glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
static float angle = 0.0f;
static glm::quat rot(cos(angle / 2.0f), sin(angle / 2.0f)*glm::vec3(0.0f, 1.0f, 0.0f));

//Matrices
static glm::mat4 projection = glm::perspective(PI / 4.0f, (float)1280 / 720, 0.1f, 1000.0f);
static glm::mat4 view;
static glm::mat4 world;

static glm::vec2 lastCursorPosition;

//voxel stuff
static GLuint voxelTexture;
static int voxelGridResolution = 64;
static int voxelGridSize = 5;

static void voxelize(int32 windowWidth, int32 windowHeight)
{
    pipeline.useProgramStage(voxelGeom);
    pipeline.clearProgramStage(voxelGeom);
    pipeline.useProgramStage(voxelFrag);
    pipeline.useProgramStage(voxelVert);

    glViewport(0, 0, voxelGridResolution, voxelGridResolution);
    glDisable(GL_DEPTH_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    float halfGridSize = voxelGridSize / 2.0f;
    glm::mat4 orthoProj = glm::ortho(-halfGridSize, halfGridSize, -halfGridSize, halfGridSize, 0.0f, (float)voxelGridSize);
    glm::mat4 orthoView = glm::inverse(glm::translate(glm::mat4(), glm::vec3(0, 0, halfGridSize)));
    glProgramUniformMatrix4fv(voxelVert.id, 5, 1, GL_FALSE, &orthoProj[0][0]);
    glProgramUniformMatrix4fv(voxelVert.id, 6, 1, GL_FALSE, &orthoView[0][0]);
    glProgramUniformMatrix4fv(voxelVert.id, 7, 1, GL_FALSE, &world[0][0]);
    glProgramUniform1i(voxelFrag.id, 9, voxelGridResolution);
    glBindImageTexture(0, voxelTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);

    glDrawElementsInstanced(
        GL_TRIANGLES,
        (GLsizei)(indexBufferSize / sizeof(uint32)),
        GL_UNSIGNED_INT,
        0,
        1);

    pipeline.clearProgramStage(voxelVert);
    pipeline.clearProgramStage(voxelFrag);
    pipeline.clearProgramStage(voxelGeom);

    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_DEPTH_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
}

static float vvd;

static void visualizeVoxelGrid()
{
    glBindVertexArray(vaos[1]);
    cubeVertexBuffer.bind();

    pipeline.useProgramStage(visualizeVoxelVert);
    pipeline.useProgramStage(visualizeVoxelFrag);

    glProgramUniformMatrix4fv(visualizeVoxelVert.id, 5, 1, GL_FALSE, &projection[0][0]);
    glProgramUniformMatrix4fv(visualizeVoxelVert.id, 6, 1, GL_FALSE, &view[0][0]);
    glProgramUniformMatrix4fv(visualizeVoxelVert.id, 7, 1, GL_FALSE, &glm::mat4()[0][0]);
    glProgramUniform1i(visualizeVoxelVert.id, 8, voxelGridSize);
    glProgramUniform1i(visualizeVoxelVert.id, 9, voxelGridResolution);
    glBindImageTexture(0, voxelTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);

    int voxelCount = voxelGridResolution*voxelGridResolution*voxelGridResolution;

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    GPU_TIMER_START(vvd);
    glDrawElementsInstanced(
        GL_TRIANGLES,
        (GLsizei)(cubeIndexBufferSize / sizeof(uint32)),
        GL_UNSIGNED_INT,
        0,
        voxelCount);
    GPU_TIMER_END(vvd);

    vvd = GPU_TIMER_GET(vvd);

    glBindVertexArray(vaos[0]);
}

static void clearVoxels()
{
    glDeleteTextures(1, &voxelTexture);
    glCreateTextures(GL_TEXTURE_3D, 1, &voxelTexture);
    glTextureStorage3D(voxelTexture, 1, GL_RGBA32F, voxelGridResolution, voxelGridResolution, voxelGridResolution);
}

void PBRApp::init()
{
    int32 imageLimit;
    glGetIntegerv(GL_MAX_VERTEX_IMAGE_UNIFORMS, &imageLimit);
    Log::info("VERTEX IMAGE UNIFORM LIMIT: %d\n", imageLimit);

    windowSize = window->getSize();

    Assets::scanAssetDirectory("assets/");
    
    glGenVertexArrays(2, vaos);
    glBindVertexArray(vaos[0]);

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

    vivi_color.bind(1);
    texture2.bind(2);
    cubemap.bind(3);

    pipeline.create();
    pipeline2.create();

    basicVert.create(Assets::getPath("basic.vert"), GL_VERTEX_SHADER);
    basicFrag.create(Assets::getPath("basic.frag"), GL_FRAGMENT_SHADER);
    CubemapVert.create(Assets::getPath("cubemap.vert"), GL_VERTEX_SHADER);
    CubemapFrag.create(Assets::getPath("cubemap.frag"), GL_FRAGMENT_SHADER);
    voxelGeom.create(Assets::getPath("voxel.geom"), GL_GEOMETRY_SHADER);
    voxelFrag.create(Assets::getPath("voxel.frag"), GL_FRAGMENT_SHADER);
    voxelVert.create(Assets::getPath("voxel.vert"), GL_VERTEX_SHADER);
    visualizeVoxelVert.create(Assets::getPath("visualize_voxel.vert"), GL_VERTEX_SHADER);
    visualizeVoxelFrag.create(Assets::getPath("visualize_voxel.frag"), GL_FRAGMENT_SHADER);

    pipeline.useProgramStage(basicVert);
    pipeline.useProgramStage(basicFrag);
    pipeline.useProgramStage(voxelGeom);

    pipeline2.useProgramStage(CubemapVert);
    pipeline2.useProgramStage(CubemapFrag);

    uint32* indices;
    uint32 vertexBufferSize;
    float* dataBof = loadBOF(Assets::getPath("vivi.bof"), &indices, &vertexBufferSize, &indexBufferSize);
    vertexBuffer.create(dataBof, vertexBufferSize);
    indexBuffer.create(indices, indexBufferSize);

    delete[] dataBof;
    delete[] indices;

    vertexBuffer.bind();
    indexBuffer.bind();

    uint32 stride = 8 * sizeof(float);
    vertexBuffer.enableVexterAttribute(0, 3, GL_FLOAT, false, stride, 0);                           //position
    vertexBuffer.enableVexterAttribute(1, 3, GL_FLOAT, false, stride, (void *)(3 * sizeof(float))); //normal
    vertexBuffer.enableVexterAttribute(2, 2, GL_FLOAT, false, stride, (void *)(6 * sizeof(float))); //texture coord

    glBindVertexArray(vaos[1]);

    dataBof = loadBOF(Assets::getPath("unit_cube.bof"), &indices, &cubeVertexBufferSize, &cubeIndexBufferSize);
    cubeVertexBuffer.create(dataBof, cubeVertexBufferSize);
    cubeIndexBuffer.create(indices, cubeIndexBufferSize);

    cubeVertexBuffer.bind();
    cubeIndexBuffer.bind();

    cubeVertexBuffer.enableVexterAttribute(0, 3, GL_FLOAT, false, stride, 0);                           //position
    cubeVertexBuffer.enableVexterAttribute(1, 3, GL_FLOAT, false, stride, (void *)(3 * sizeof(float))); //normal
    cubeVertexBuffer.enableVexterAttribute(2, 2, GL_FLOAT, false, stride, (void *)(6 * sizeof(float))); //texture coord

    //Voxelization
    glGenTextures(1, &voxelTexture);
    glBindTexture(GL_TEXTURE_3D, voxelTexture);
    glTextureStorage3D(voxelTexture, 1, GL_RGBA32F, voxelGridResolution, voxelGridResolution, voxelGridResolution);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void PBRApp::update(float dt)
{
    static float time = 0.0f;
    time += dt;

    CHECK_FOR_SHADER_UPDATE(basicVert);
    CHECK_FOR_SHADER_UPDATE(basicFrag);
    CHECK_FOR_SHADER_UPDATE(voxelGeom);
    CHECK_FOR_SHADER_UPDATE(voxelFrag);
    CHECK_FOR_SHADER_UPDATE(voxelVert);
    CHECK_FOR_SHADER_UPDATE(visualizeVoxelVert);
    CHECK_FOR_SHADER_UPDATE(visualizeVoxelFrag);

    //pos.y = sin(time);
    glm::vec3 scale = glm::vec3(scaleAmount, scaleAmount, scaleAmount);
    glm::vec3 rotation = glm::vec3(0.0f, 1.0f, 0.0f);
    world = glm::rotate(glm::scale(glm::translate(glm::mat4(), pos), scale), rotationAmount, rotation);

    glm::vec2 delta(0);
    int32 status = glfwGetMouseButton(window->handle->ptr, GLFW_MOUSE_BUTTON_RIGHT);
    if (status == GLFW_PRESS)
    {
        glm::vec2 newCursorPosition(Input::mousePosition.x, Input::mousePosition.y);
        delta = lerp(lastCursorPosition, newCursorPosition, 0.1f * dt) - lastCursorPosition;
    }
    lastCursorPosition = glm::vec2(Input::mousePosition.x, Input::mousePosition.y);

    float movementSpeed = dt * 400.0f;
    if ((glfwGetKey(window->handle->ptr, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
    {
        movementSpeed *= 0.1f;
    }

    int32 key_w = (glfwGetKey(window->handle->ptr, GLFW_KEY_W) == GLFW_PRESS);
    int32 key_a = (glfwGetKey(window->handle->ptr, GLFW_KEY_A) == GLFW_PRESS);
    int32 key_s = (glfwGetKey(window->handle->ptr, GLFW_KEY_S) == GLFW_PRESS);
    int32 key_d = (glfwGetKey(window->handle->ptr, GLFW_KEY_D) == GLFW_PRESS);
    int32 key_q = (glfwGetKey(window->handle->ptr, GLFW_KEY_Q) == GLFW_PRESS);
    int32 key_e = (glfwGetKey(window->handle->ptr, GLFW_KEY_E) == GLFW_PRESS);

    fpsCamera(rot, cameraPos, delta, movementSpeed, dt, key_w, key_a, key_s, key_d, key_q, key_e);
    
    view = glm::inverse(glm::translate(glm::mat4(), cameraPos) * glm::mat4_cast(rot));

    if (GLFW_PRESS == glfwGetKey(window->handle->ptr, GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(window->handle->ptr, 1);
    }
}

void PBRApp::render(float dt)
{
    static bool visualizeVoxels = true;

    GPU_TIMER_START(frame);

    glViewport(0, 0, windowSize.x, windowSize.y);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LEQUAL);

    //glClearColor(0.5f*sin(time)+0.5f, 0.5f*cos(1.5f+time/2.0f)+0.5f, 0.2f, 1.0f);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glProgramUniformMatrix4fv(basicVert.id, 7, 1, GL_FALSE, &world[0][0]);
    glProgramUniformMatrix4fv(basicVert.id, 6, 1, GL_FALSE, &view[0][0]);
    glProgramUniformMatrix4fv(basicVert.id, 5, 1, GL_FALSE, &projection[0][0]);

    glProgramUniformMatrix4fv(CubemapVert.id, 5, 1, GL_FALSE, &projection[0][0]);
    glProgramUniformMatrix4fv(CubemapVert.id, 6, 1, GL_FALSE, &view[0][0]);

    glBindProgramPipeline(pipeline2.id);

    //debug only
    pipeline2.useProgramStage(CubemapVert);
    pipeline2.useProgramStage(CubemapFrag);

    //cubemap
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindProgramPipeline(pipeline.id);

    pipeline.useProgramStage(basicVert);
    pipeline.useProgramStage(basicFrag);

    glBindVertexArray(vaos[0]);
    vertexBuffer.bind();

    GPU_TIMER_START(model);
        glDrawElementsInstanced(
            GL_TRIANGLES,
            (GLsizei)(indexBufferSize / sizeof(uint32)),
            GL_UNSIGNED_INT,
            0,
            1);
    GPU_TIMER_END(model);

    GPU_TIMER_START(voxelize);
    voxelize(windowSize.x, windowSize.y);
    GPU_TIMER_END(voxelize);

    GPU_TIMER_START(visualize);
    if (visualizeVoxels)
        visualizeVoxelGrid();
    GPU_TIMER_END(visualize);

    clearVoxels();

    GPU_TIMER_END(frame);

    const int32 frames = 100;
    static float averageFrameRate = 0.0f;
    static float rollingFrametimeCounter[frames] = { 0.0f };
    static int32 currentFrame = 0;
    averageFrameRate += dt - rollingFrametimeCounter[currentFrame];
    rollingFrametimeCounter[currentFrame] = dt;
    currentFrame = ++currentFrame % frames;

    static float time = 0;
    time += dt;

    ui.update(window);

    {
        float panelWidth = 300.0f;
        ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f));
        ImGui::Begin("Stats", 0, ImVec2(panelWidth, 0), 0.3f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
        ImGui::ProgressBar(GPU_TIMER_GET(frame) / 16.666f, ImVec2(-1.0f, 3.0f), "");
        ImGui::Text("GPU Frame Time %.2f ms", GPU_TIMER_GET(frame));
        ImGui::Text("  Voxelization %.2f ms", GPU_TIMER_GET(voxelize));
        ImGui::Text("  Voxel Draw %.2f ms", GPU_TIMER_GET(visualize));
        ImGui::Text("  Voxel Draw Only %.2f ms", vvd);
        ImGui::Text("  Model Draw %.2f ms", GPU_TIMER_GET(model));
        //ImGui::Text("dt %.3f ms, %.1f FPS", dt * 1000.0f, 1.0f / (averageFrameRate / frames));
        //ImGui::Text("mouse x: %.2f, y: %.2f", Input::mousePosition.x, Input::mousePosition.y);
        ImGui::Text("%.1f s", time);
        ImGui::End();
    }

    {
        ImGui::Begin("Voxels");
        ImGui::Text("%dx%dx%d - %d",
            voxelGridResolution,
            voxelGridResolution,
            voxelGridResolution,
            voxelGridResolution*voxelGridResolution*voxelGridResolution);
        ImGui::Checkbox("Visualize", &visualizeVoxels);
        if (ImGui::Button("Clear"))
            clearVoxels();
        ImGui::End();
    }

    ImGui::Render();
    ui.render(ImGui::GetDrawData());
}
