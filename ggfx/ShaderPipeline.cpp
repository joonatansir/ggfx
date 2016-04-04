#include <GL/gl3w.h>

#include "ShaderPipeline.h"
#include "util.h"
#include "Log.h"

using namespace ggfx;

ShaderPipeline::ShaderPipeline(Shader vertex, Shader fragment) :
    vertexShader(vertex),
    fragmentShader(fragment)
{
}

ShaderPipeline::ShaderPipeline()
{
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
    recompileShader(vertexShader, GL_VERTEX_SHADER_BIT);
    recompileShader(fragmentShader, GL_FRAGMENT_SHADER_BIT);
    Log::info("Pipeline %u recompiled!\n", id);
}

void ShaderPipeline::recompileShader(Shader& shader, uint32 stages)
{
    glDeleteProgram(shader.id);
    shader = createShaderProgram(shader.filename, shader.type);
    useProgramStage(shader.id, stages);
}

Shader ShaderPipeline::createShaderProgram(const char* filename, uint32 type)
{
    Shader shader = { 0, type, filename };
    const uint8* source = loadFile(filename);
    shader.id = glCreateShaderProgramv(type, 1, (const char**)&source);
    delete[] source;

    //Error checking
    char buffer[512] = {0};
    int32 length;
    glGetProgramInfoLog(shader.id, 512, &length, buffer);

    if(length > 0)
        Log::error(buffer);

    return shader;
}

ShaderPipeline ShaderPipeline::createPipeline(const char* vertexSource, const char* fragmentSource)
{
    Shader vertexShader = createShaderProgram(vertexSource, GL_VERTEX_SHADER);
    Shader fragmentShader = createShaderProgram(fragmentSource, GL_FRAGMENT_SHADER);

    return createPipeline(vertexShader, fragmentShader);
}

ShaderPipeline ShaderPipeline::createPipeline(const Shader & vertexShader, const Shader & fragmentShader)
{
    ShaderPipeline pipeline(vertexShader, fragmentShader);

    glGenProgramPipelines(1, &pipeline.id);
    glBindProgramPipeline(pipeline.id);

    pipeline.useProgramStage(pipeline.vertexShader.id, GL_VERTEX_SHADER_BIT);
    pipeline.useProgramStage(pipeline.fragmentShader.id, GL_FRAGMENT_SHADER_BIT);

    glBindProgramPipeline(0);

    return pipeline;
}
