#pragma once

#include "types.h"

namespace ggfx
{
    struct Shader
    {
        uint32 id;
        uint32 type;
        const char* filename; //Store filename for recompiling

        int32 getUniformLocation(const char* name)
        {
            return glGetUniformLocation(id, name);
        }
    };

    class ShaderPipeline
    {
    public:

        ~ShaderPipeline();

        void useProgramStage(uint32 shaderID, uint32 stages);
        void recompile();
        void recompileShader(Shader& shader, uint32 stages);

        static Shader createProgramFromFile(const char* filename, uint32 type);
        static ShaderPipeline createPipeline(Shader vertexShader, Shader fragmentShader);

        uint32 id;
        Shader vs;
        Shader fs;

    private:

        ShaderPipeline(Shader vertex, Shader fragment);

    };
}