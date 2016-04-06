#pragma once

#if defined(_DEBUG)

    #include "Log.h"
    #include "ShaderPipeline.h"

    #define CHECK_FOR_SHADER_UPDATE(x) checkForShaderUpdate(x)

    void checkForShaderUpdate(ggfx::Shader& shader);

#else
    #define CHECK_FOR_SHADER_UPDATE(x)
#endif
