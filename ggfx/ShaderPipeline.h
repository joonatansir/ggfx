#pragma once

#include <string>

#include <GL/gl3w.h>

#include "types.h"
#include "Log.h"
#include "Shader.h"

namespace ggfx
{
    class ShaderPipeline
    {
    public:

        ShaderPipeline();
        ~ShaderPipeline();

        void useProgramStage(Shader& shader);
        void create();

        uint32 id;
    };
}