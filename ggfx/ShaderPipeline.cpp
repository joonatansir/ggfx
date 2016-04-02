#include <GL/gl3w.h>

#include "ShaderPipeline.h"
#include "util.h"
#include "Log.h"

using namespace ggfx;

ShaderPipeline::ShaderPipeline(Shader vertex, Shader fragment) :
    vs(vertex),
    fs(fragment)
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
    recompileShader(vs, GL_VERTEX_SHADER_BIT);
    recompileShader(fs, GL_FRAGMENT_SHADER_BIT);
    Log::print("Pipeline %u recompiled!\n", id);
}

void ShaderPipeline::recompileShader(Shader& shader, uint32 stages)
{
    glDeleteProgram(shader.id);
    shader = createProgramFromFile(shader.filename, shader.type);
    useProgramStage(shader.id, stages);
}

Shader ShaderPipeline::createProgramFromFile(const char* filename, uint32 type)
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
        Log::print(buffer);

    return shader;
}

ShaderPipeline ShaderPipeline::createPipeline(Shader vertexShader, Shader fragmentShader)
{
    ShaderPipeline pipeline(vertexShader, fragmentShader);

    glGenProgramPipelines(1, &pipeline.id);
    glBindProgramPipeline(pipeline.id);

    pipeline.useProgramStage(pipeline.vs.id, GL_VERTEX_SHADER_BIT);
    pipeline.useProgramStage(pipeline.fs.id, GL_FRAGMENT_SHADER_BIT);
    
    glBindProgramPipeline(0);

    return pipeline;
}
