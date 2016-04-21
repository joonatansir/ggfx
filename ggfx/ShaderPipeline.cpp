#include <string>

#include <GL/gl3w.h>

#include "ShaderPipeline.h"
#include "util.h"
#include "Log.h"
#include "Assets.h"

using namespace ggfx;

ShaderPipeline::ShaderPipeline() :
    id(0)
{
}

ShaderPipeline::~ShaderPipeline()
{
    glDeleteProgramPipelines(1, &id);
}

void ShaderPipeline::useProgramStage(Shader& shader)
{
    assert(id != 0);
    shader.info.pipeline = this;
    glUseProgramStages(id, shader.info.stages, shader.id);
}

void ShaderPipeline::create()
{
    glGenProgramPipelines(1, &id);
}
