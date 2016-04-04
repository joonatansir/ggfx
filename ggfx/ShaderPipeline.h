#pragma once

#include "types.h"

namespace ggfx
{
    struct Shader
    {
        uint32 id;
        uint32 type;
        const char* filename; //Store filename for recompiling

        inline int32 getUniformLocation(const char* name)
        {
            return glGetUniformLocation(id, name);
        }
    };

    class ShaderPipeline
    {
    public:

        ShaderPipeline();
        ~ShaderPipeline();

        void useProgramStage(uint32 shaderID, uint32 stages);
        void recompile();
        void recompileShader(Shader& shader, uint32 stages);

        static Shader createShaderProgram(const char* filename, uint32 type);
        static ShaderPipeline createPipeline(const char* vertexShader, const char* fragmentShader);
        static ShaderPipeline createPipeline(const Shader& vertexShader, const Shader& fragmentShader);

        uint32 id;
        Shader vertexShader;
        Shader fragmentShader;

    private:

        ShaderPipeline(Shader vertex, Shader fragment);

    };
}