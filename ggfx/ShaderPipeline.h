#pragma once

#include <string>

#include <GL/gl3w.h>

#include "types.h"
#include "Log.h"

namespace ggfx
{
    class ShaderPipeline;

    struct Shader
    {
        uint32 id;
        uint32 type;
        int32* uniformLocations[16];
        char* uniformNames[16][100];
        uint32 uniformCount;

        struct
        {
            uint64 lastModified;
            std::string filename;
            ShaderPipeline* pipeline;
        } info;

        void getUniformLocation(int32* uniformLocation, char* name)
        {
            if (uniformCount >= 16)
            {
                Log::warning("Max uniforms exceeded!");
                return;
            }

            *uniformLocation = glGetUniformLocation(id, name);
            uniformLocations[uniformCount] = uniformLocation;
            uniformNames[uniformCount][0] = name;
            uniformCount++;
        }
    };

    class ShaderPipeline
    {
    public:

        ShaderPipeline();
        ShaderPipeline& operator =(ShaderPipeline& pipeline);
        ~ShaderPipeline();

        void useProgramStage(uint32 shaderID, uint32 stages);
        void recompile();
        void recompileShader(Shader& shader, uint32 stages);

        static void recreateShaderProgram(Shader& shader);
        static Shader createShaderProgram(const std::string& filename, uint32 type);
        static ShaderPipeline createPipelineFromFile(const std::string& filename);

        uint32 id;
        Shader vertexShader;
        Shader fragmentShader;
    };
}