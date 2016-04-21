#pragma once

#include <string>

#include <GL/gl3w.h>

#include "types.h"
#include "Log.h"

namespace ggfx
{
    class ShaderPipeline;

    class Shader
    {
    public:

        Shader();
        Shader(const std::string& filename, GLenum type);
        ~Shader();

        void create(const std::string& filename, GLenum type);
        void getUniformLocation(GLint* uniformLocation, char* name);
        void recompile();
        
        uint32 id;
        uint32 type;
        int32* uniformLocations[16];
        char* uniformNames[16][100];
        uint32 uniformCount;

        struct
        {
            uint32 stages;
            ShaderPipeline* pipeline;
            uint64 lastModified;
            std::string filename;
        } info;
    };
}