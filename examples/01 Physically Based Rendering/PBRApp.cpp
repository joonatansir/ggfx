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
#include "GLhelper.h"

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
static Shader physicallyBased;

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
static glm::vec3 pos = glm::vec3(0.0f, -10.0f, 0.0f);

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
static int voxelGridResolution = 128;
static int voxelGridWorldSize = 2;

//voxelize renderbuffers
GLuint voxelizeColorRenderbuffer;
GLuint voxelizeDepthRenderbuffer;
GLuint voxelizeFBO;
static GLenum voxelImageFormat = GL_RGBA8;

static void voxelize(int32 windowWidth, int32 windowHeight)
{
    pipeline.useProgramStage(voxelGeom);
    pipeline.useProgramStage(voxelFrag);
    pipeline.useProgramStage(voxelVert);

    glBindFramebuffer(GL_FRAMEBUFFER, voxelizeFBO);

    glViewport(0, 0, voxelGridResolution, voxelGridResolution);
    glDisable(GL_DEPTH_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    float halfGridSize = voxelGridWorldSize / 2.0f;
    glm::mat4 orthoProj = glm::ortho(-halfGridSize, halfGridSize, -halfGridSize, halfGridSize, 0.0f, (float)voxelGridWorldSize);
    glm::mat4 orthoView = glm::inverse(glm::translate(glm::mat4(), glm::vec3(0, 0, halfGridSize)));
    glm::mat3 projMatrices[] = { glm::mat3(0.0, 0.0, 1.0,
                                           0.0, 1.0, 0.0,
                                           1.0, 0.0, 0.0),
                                 glm::mat3(1.0, 0.0, 0.0,
                                           0.0, 0.0, 1.0,
                                           0.0, 1.0, 0.0),
                                 glm::mat3(1.0, 0.0, 0.0,
                                           0.0, 1.0, 0.0,
                                           0.0, 0.0, 1.0) };
    glProgramUniformMatrix4fv(voxelVert.id, 5, 1, GL_FALSE, &orthoProj[0][0]);
    glProgramUniformMatrix4fv(voxelVert.id, 6, 1, GL_FALSE, &orthoView[0][0]);
    glProgramUniformMatrix4fv(voxelVert.id, 7, 1, GL_FALSE, &world[0][0]);
    glProgramUniformMatrix3fv(voxelGeom.id, 15, 3, GL_FALSE, &projMatrices[0][0][0]);
    glProgramUniform1i(voxelFrag.id, 9, voxelGridResolution);

    glBindImageTexture(0, voxelTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI);

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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void visualizeVoxelGrid()
{
    glBindVertexArray(vaos[1]);
    cubeVertexBuffer.bind();

    pipeline.clearProgramStage(GL_GEOMETRY_SHADER_BIT);
    pipeline.useProgramStage(visualizeVoxelVert);
    pipeline.useProgramStage(visualizeVoxelFrag);

    glProgramUniformMatrix4fv(visualizeVoxelVert.id, 5, 1, GL_FALSE, &projection[0][0]);
    glProgramUniformMatrix4fv(visualizeVoxelVert.id, 6, 1, GL_FALSE, &view[0][0]);
    glProgramUniformMatrix4fv(visualizeVoxelVert.id, 7, 1, GL_FALSE, &glm::mat4()[0][0]);
    glProgramUniform1i(visualizeVoxelVert.id, 8, voxelGridWorldSize);
    glProgramUniform1i(visualizeVoxelVert.id, 9, voxelGridResolution);

    glBindImageTexture(0, voxelTexture, 0, GL_TRUE, 0, GL_READ_WRITE, voxelImageFormat);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    int voxelCount = voxelGridResolution*voxelGridResolution*voxelGridResolution;
    glDrawElementsInstanced(
        GL_TRIANGLES,
        (GLsizei)(cubeIndexBufferSize / sizeof(uint32)),
        GL_UNSIGNED_INT,
        0,
        voxelCount);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindVertexArray(vaos[0]);
}

static void clearVoxels()
{
    glDeleteTextures(1, &voxelTexture);
    glCreateTextures(GL_TEXTURE_3D, 1, &voxelTexture);
    glTextureStorage3D(voxelTexture, 1, voxelImageFormat, voxelGridResolution, voxelGridResolution, voxelGridResolution);
}

void PBRApp::init()
{
    windowSize = window->getSize();

    Assets::scanAssetDirectory("assets/");
    
    GL_PRINT(GL_MAX_SAMPLES);

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
    physicallyBased.create(Assets::getPath("physicallyBased.frag"), GL_FRAGMENT_SHADER);

    pipeline2.useProgramStage(CubemapVert);
    pipeline2.useProgramStage(CubemapFrag);

    uint32* indices;
    uint32 vertexBufferSize;
    float* dataBof = loadBOF(Assets::getPath("sphere.bof"), &indices, &vertexBufferSize, &indexBufferSize);
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

    dataBof = loadBOF(Assets::getPath("cube_uv.bof"), &indices, &cubeVertexBufferSize, &cubeIndexBufferSize);
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
    glTextureStorage3D(voxelTexture, 1, voxelImageFormat, voxelGridResolution, voxelGridResolution, voxelGridResolution);

    glGenRenderbuffers(1, &voxelizeColorRenderbuffer);
    glGenRenderbuffers(1, &voxelizeDepthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, voxelizeColorRenderbuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, voxelImageFormat, voxelGridResolution, voxelGridResolution);
    glBindRenderbuffer(GL_RENDERBUFFER, voxelizeDepthRenderbuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH_COMPONENT32F, voxelGridResolution, voxelGridResolution);

    glGenFramebuffers(1, &voxelizeFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, voxelizeFBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, voxelizeColorRenderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, voxelizeDepthRenderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        LOG(info, "FBO Complete\n");
    else
        LOG(info, "FBO Not Complete");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static bool animateModel = false;

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
    CHECK_FOR_SHADER_UPDATE(physicallyBased);

    static float upDownTimer = 0.0f;
    if (animateModel)
    {
        upDownTimer += dt;
        pos.y = sin(upDownTimer);
        rotationAmount += 0.01f;
    }
    glm::vec3 scale = glm::vec3(scaleAmount, scaleAmount, scaleAmount);
    glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    world = glm::rotate(glm::scale(glm::translate(glm::mat4(), pos), scale), rotationAmount, rotationAxis);

    glm::vec2 delta(0);
    int32 status = glfwGetMouseButton(window->handle->ptr, GLFW_MOUSE_BUTTON_RIGHT);
    if (status == GLFW_PRESS)
    {
        glm::vec2 newCursorPosition(Input::mousePosition.x, Input::mousePosition.y);
        delta = lerp(lastCursorPosition, newCursorPosition, 0.1f * dt) - lastCursorPosition;
    }
    lastCursorPosition = glm::vec2(Input::mousePosition.x, Input::mousePosition.y);

    float movementSpeed = dt * 50.0f;
    if ((glfwGetKey(window->handle->ptr, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
    {
        movementSpeed *= 0.5f;
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
    static bool showModel = true;

    GPU_TIMER_START(frame);

    glViewport(0, 0, windowSize.x, windowSize.y);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LEQUAL);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);

    //glClearColor(0.5f*sin(time)+0.5f, 0.5f*cos(1.5f+time/2.0f)+0.5f, 0.2f, 1.0f);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glProgramUniformMatrix4fv(basicVert.id, 7, 1, GL_FALSE, &world[0][0]);
    glProgramUniformMatrix4fv(basicVert.id, 6, 1, GL_FALSE, &view[0][0]);
    glProgramUniformMatrix4fv(basicVert.id, 5, 1, GL_FALSE, &projection[0][0]);

    glProgramUniformMatrix4fv(CubemapVert.id, 5, 1, GL_FALSE, &projection[0][0]);
    glProgramUniformMatrix4fv(CubemapVert.id, 6, 1, GL_FALSE, &view[0][0]);

    glProgramUniform3fv(physicallyBased.id, 3, 1, &cameraPos[0]);

    glBindProgramPipeline(pipeline2.id);

    //debug only
    pipeline2.useProgramStage(CubemapVert);
    pipeline2.useProgramStage(CubemapFrag);

    //cubemap
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindProgramPipeline(pipeline.id);

    pipeline.clearAllStages();
    pipeline.useProgramStage(basicVert);
    pipeline.useProgramStage(physicallyBased);

    glBindVertexArray(vaos[0]);
    vertexBuffer.bind();

    GPU_TIMER_START(model);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if(showModel)
        glDrawElementsInstanced(
            GL_TRIANGLES,
            (GLsizei)(indexBufferSize / sizeof(uint32)),
            GL_UNSIGNED_INT,
            0,
            1);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    GPU_TIMER_END(model);

    static bool v = true;
    GPU_TIMER_START(voxelize);
    if (v)
    {
        voxelize(windowSize.x, windowSize.y);
        v = true;
    }
    GPU_TIMER_END(voxelize);
    
    GPU_TIMER_START(visualize);
    if (visualizeVoxels)
        visualizeVoxelGrid();
    GPU_TIMER_END(visualize);

    glFlush();

    GPU_TIMER_START(clearVoxels);
    clearVoxels();
    GPU_TIMER_END(clearVoxels);
    
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
        float panelWidth = 250.0f;
        ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f));
        ImGui::Begin("Stats", 0, ImVec2(panelWidth, 0), 0.3f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
        ImGui::ProgressBar(GPU_TIMER_GET(frame) / 16.666f, ImVec2(-1.0f, 5.0f), "");
        ImGui::Text("\nGPU Frame Time %.2f ms", GPU_TIMER_GET(frame));
        ImGui::Text(" > Voxelization %.2f ms", GPU_TIMER_GET(voxelize));
        ImGui::Text(" > Voxel Draw %.2f ms", GPU_TIMER_GET(visualize));
        ImGui::Text(" > Model Draw %.2f ms", GPU_TIMER_GET(model));
        ImGui::Text("dt %.3f ms, %.1f FPS", dt * 1000.0f, 1.0f / (averageFrameRate / frames));
        ImGui::Text("mouse x: %.2f, y: %.2f", Input::mousePosition.x, Input::mousePosition.y);
        ImGui::Text("\nTime: %.1f s", time);
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
        ImGui::SameLine();
        ImGui::Checkbox("Show Model", &showModel);
        if (ImGui::Button("Clear"))
            clearVoxels();
        ImGui::End();
    }

    {
        ImGui::Begin("Model");
        ImGui::Checkbox("Animate", &animateModel);
        ImGui::End();
    }

    ImGui::Render();
    ui.render(ImGui::GetDrawData());
}
