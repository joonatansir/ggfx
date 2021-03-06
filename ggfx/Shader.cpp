#include "Shader.h"
#include "util.h"
#include "ShaderPipeline.h"
#include "Assets.h"

using namespace ggfx;

#if defined(_DEBUG)
#define CHECK_PROGRAM_INFO_LOG(id, filename)                   \
    char buffer[512] = { 0 };                        \
    int32 length;                                    \
    glGetProgramInfoLog((id), 512, &length, buffer); \
    if (length > 0) Log::info("Problem compiling %s:%s", filename, buffer);               \

#else
#define CHECK_PROGRAM_INFO_LOG(id)
#endif

Shader::Shader() :
    id(0),
    info{0, 0, 0, ""}
{
}

Shader::Shader(const std::string& filename, GLenum type) :
    info{0, 0, 0, ""}
{
    create(filename, type);
}

Shader::~Shader()
{
    glDeleteProgram(id);
}

void Shader::create(const std::string& filename, GLenum type)
{
    this->type = type;
    info.filename = filename;

    const uint8* source = loadFile(info.filename);
    id = glCreateShaderProgramv(type, 1, (const char**)&source);
    delete[] source;
    CHECK_PROGRAM_INFO_LOG(id, filename.c_str());

    switch (type)
    {
    case GL_VERTEX_SHADER:
        info.stages = GL_VERTEX_SHADER_BIT;
        break;
    case GL_FRAGMENT_SHADER:
        info.stages = GL_FRAGMENT_SHADER_BIT;
        break;
    case GL_GEOMETRY_SHADER:
        info.stages = GL_GEOMETRY_SHADER_BIT;
        break;
    }
}

void Shader::getUniformLocation(GLint* uniformLocation, char* name)
{
    if (uniformCount >= 16)
    {
        LOG(error, "Max uniforms exceeded!");
        return;
    }

    *uniformLocation = glGetUniformLocation(id, name);
    uniformLocations[uniformCount] = uniformLocation;
    uniformNames[uniformCount][0] = name;
    uniformCount++;
}

void Shader::recompile()
{
    LOG(info, "recompiling %s!\n", info.filename.c_str());

    glDeleteProgram(id);
    create(info.filename, type);
    
    if (info.pipeline && info.pipeline->id != 0)
    {
        glBindProgramPipeline(info.pipeline->id);
        info.pipeline->useProgramStage(*this);
    }

    for (uint32 i = 0; i < uniformCount; i++)
    {
        *uniformLocations[i] = glGetUniformLocation(id, uniformNames[i][0]);
    }
}
