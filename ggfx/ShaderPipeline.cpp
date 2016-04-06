#include <string>

#include <GL/gl3w.h>

#include "ShaderPipeline.h"
#include "util.h"
#include "Log.h"
#include "Assets.h"

using namespace ggfx;

#if defined(_DEBUG)
#define CHECK_PROGRAM_INFO_LOG(id)                   \
    char buffer[512] = { 0 };                        \
    int32 length;                                    \
    glGetProgramInfoLog((id), 512, &length, buffer); \
    if (length > 0) Log::info(buffer);              \

#else
#define CHECK_PROGRAM_INFO_LOG(id)
#endif

static void LinkUniformLocations(Shader& shader)
{
    for (uint32 i = 0; i < shader.uniformCount; i++)
    {
        *shader.uniformLocations[i] = glGetUniformLocation(shader.id, shader.uniformNames[i][0]);
    }
}

ShaderPipeline::ShaderPipeline()
{
}

ShaderPipeline& ShaderPipeline::operator=(ShaderPipeline& pipeline)
{
    id = pipeline.id;
    vertexShader = pipeline.vertexShader;
    fragmentShader = pipeline.fragmentShader;
    vertexShader.info.pipeline = this;
    fragmentShader.info.pipeline = this;
    return *this;
}

ShaderPipeline::~ShaderPipeline()
{
}

void ShaderPipeline::useProgramStage(uint32 shaderID, uint32 stages)
{
    glUseProgramStages(id, stages, shaderID);
}

void ShaderPipeline::recompile()
{
    glBindProgramPipeline(id);
    recompileShader(vertexShader, GL_VERTEX_SHADER_BIT);
    recompileShader(fragmentShader, GL_FRAGMENT_SHADER_BIT);

    //Get new uniform locations
    LinkUniformLocations(vertexShader);
    LinkUniformLocations(fragmentShader);

    Log::info("Pipeline %u recompiled!\n", id);
}

void ShaderPipeline::recompileShader(Shader& shader, uint32 stages)
{
    glDeleteProgram(shader.id);
    recreateShaderProgram(shader);
    useProgramStage(shader.id, stages);
}

void ShaderPipeline::recreateShaderProgram(Shader& shader)
{
    const uint8* source = loadFile(shader.info.filename);
    shader.id = glCreateShaderProgramv(shader.type, 1, (const char**)&source);
    delete[] source;
    CHECK_PROGRAM_INFO_LOG(shader.id);
}

Shader ShaderPipeline::createShaderProgram(const std::string& filename, uint32 type)
{
    Shader shader;
    shader.type = type;
    shader.info.lastModified = 0;
    shader.info.filename = filename;
    shader.uniformCount = 0;

    const uint8* source = loadFile(filename);
    shader.id = glCreateShaderProgramv(type, 1, (const char**)&source);
    delete[] source;
    CHECK_PROGRAM_INFO_LOG(shader.id);

    return shader;
}

ShaderPipeline ShaderPipeline::createPipelineFromFile(const std::string& filename)
{
    ShaderPipeline pipeline;

    std::string vertexFilename(Assets::getPath(filename + ".vert"));
    std::string fragmentFilename(Assets::getPath(filename + ".frag"));

    pipeline.vertexShader = createShaderProgram(vertexFilename, GL_VERTEX_SHADER);
    pipeline.fragmentShader = createShaderProgram(fragmentFilename, GL_FRAGMENT_SHADER);

    pipeline.vertexShader.info.pipeline = &pipeline;
    pipeline.fragmentShader.info.pipeline = &pipeline;

    glGenProgramPipelines(1, &pipeline.id);
    glBindProgramPipeline(pipeline.id);
    pipeline.useProgramStage(pipeline.vertexShader.id, GL_VERTEX_SHADER_BIT);
    pipeline.useProgramStage(pipeline.fragmentShader.id, GL_FRAGMENT_SHADER_BIT);
    glBindProgramPipeline(0);

    return pipeline;
}
