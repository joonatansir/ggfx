//
//  util.cpp
//  ggfx
//
//  Created by jon on 21/03/16.
//  Copyright © 2016 Joonatan Sörensen. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include <map>
#include <unordered_map>
#include <functional>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "util.h"
#include "types.h"
#include "Log.h"

namespace ggfx
{
    struct MeshIndex
    {
        uint32 indices[3];
    };

    static size_t hashFunction(const MeshIndex& m)
    {
        return std::hash<uint32>()(m.indices[0]) ^ 
               std::hash<uint32>()(m.indices[1]) ^
               std::hash<uint32>()(m.indices[2]);
    }

    static bool equalFunction(const MeshIndex& a, const MeshIndex& b)
    {
        return a.indices[0] == b.indices[0] &&
               a.indices[1] == b.indices[1] &&
               a.indices[2] == b.indices[2];
    }

    inline bool operator <(const MeshIndex& a, const MeshIndex& b)
    {
        int32 aValue = a.indices[0]*100 + a.indices[1]*10 + a.indices[2];
        int32 bValue = b.indices[0]*100 + b.indices[1]*10 + b.indices[2];
        return aValue < bValue;
    }

    const uint8* loadFile(const char* filename)
    {
        uint8* result = 0;
        
        FILE* file = fopen(filename, "rb");
        if(file)
        {
            fseek(file, 0, SEEK_END);
            uint32 fileSize = ftell(file);
            fseek(file, 0, SEEK_SET);
            
            result = new uint8[fileSize + 1];
            result[fileSize] = 0;
            
            size_t bytesRead = fread(result, fileSize, 1, file);
            
            fclose(file);
        }
        
        assert(result);
            
        return result;
    }

    void loadBinaryOBJHeader(const char* filename, uint32* vpBytes, uint32* vtBytes, uint32* vnBytes, uint32* idxBytes)
    {
        FILE* file = fopen(filename, "rb");

        assert(file);
        
        fread(&vpBytes, sizeof(uint32), 1, file);
        fread(&vtBytes, sizeof(uint32), 1, file);
        fread(&vnBytes, sizeof(uint32), 1, file);
        fread(&idxBytes, sizeof(uint32), 1, file);

        fclose(file);
    }

    //Read binary .obj file. Data is returned in the following format:
    //pos.x, pos.y, pos.z, normal.x, normal.y, normal.z, uv.x, uv.y
    /*float* loadBinaryOBJ(
        const char* filename, 
        uint32** indices, 
        uint32& vpSizeInBytesOut, 
        uint32& vtSizeInBytesOut, 
        uint32& vnSizeInBytesOut,
        uint32& fSizeinBytesOut)
    {
        float* result = 0;

        FILE* file = fopen(filename, "rb");
        if (file)
        {
            uint32 vpSizeInBytes;
            uint32 vtSizeInBytes;
            uint32 vnSizeInBytes;
            uint32 fSizeInBytes;

            fread(&vpSizeInBytes, sizeof(uint32), 1, file);
            fread(&vtSizeInBytes, sizeof(uint32), 1, file);
            fread(&vnSizeInBytes, sizeof(uint32), 1, file);
            fread(&fSizeInBytes, sizeof(uint32), 1, file);
            
            const uint32 attributeCount = 3;

            uint32 vpSize = vpSizeInBytes / sizeof(float);
            uint32 vtSize = vtSizeInBytes / sizeof(float);
            uint32 vnSize = vnSizeInBytes / sizeof(float);
            uint32 fSize = fSizeInBytes / sizeof(uint32);

            float* positions = new float[vpSize];
            float* uvs = new float[vtSize];
            float* normals = new float[vnSize];
            uint32* faceElements = new uint32[fSize];

            fread(positions, vpSizeInBytes, 1, file);
            fread(uvs, vtSizeInBytes, 1, file);
            fread(normals, vnSizeInBytes, 1, file);
            fread(faceElements, fSizeInBytes, 1, file);

            uint32 stride = 8;
            *indices = new uint32[fSize / attributeCount];
            result = new float[fSize / attributeCount * stride];
            
            uint32 indicesCount = fSizeInBytes / sizeof(uint32);
            uint32 indicesIndex = 0;

            std::unordered_map<
                MeshIndex, 
                int32, 
                std::function<int32(const MeshIndex&)>,
                std::function<bool(const MeshIndex&, const MeshIndex&)>> uniqueIndices(1000, hashFunction, equalFunction);

            for (uint32 i = 0, k = 0; i < indicesCount; i+=3, k++)
            {
                MeshIndex m = { { faceElements[i], faceElements[i + 1], faceElements[i + 2] } };

                size_t count = uniqueIndices.count(m);

                if (count > 0)
                {
                    (*indices)[k] = uniqueIndices[m];
                }
                else
                {
                    uniqueIndices[m] = indicesIndex;
                    (*indices)[k] = indicesIndex;

                    result[indicesIndex * stride + 0] = positions[m.indices[0] * 3 + 0];
                    result[indicesIndex * stride + 1] = positions[m.indices[0] * 3 + 1];
                    result[indicesIndex * stride + 2] = positions[m.indices[0] * 3 + 2];

                    result[indicesIndex * stride + 3] = normals[m.indices[2] * 3 + 0];
                    result[indicesIndex * stride + 4] = normals[m.indices[2] * 3 + 1];
                    result[indicesIndex * stride + 5] = normals[m.indices[2] * 3 + 2];

                    result[indicesIndex * stride + 6] = uvs[m.indices[1] * 2 + 0];
                    result[indicesIndex * stride + 7] = uvs[m.indices[1] * 2 + 1];

                    indicesIndex++;
                }
            }

            vpSizeInBytesOut = indicesIndex * stride * sizeof(float);
            vtSizeInBytesOut = vtSizeInBytes;
            vnSizeInBytesOut = vnSizeInBytes;
            fSizeinBytesOut = fSizeInBytes / attributeCount;
            
            realloc(result, vpSizeInBytesOut);

            fclose(file);
        }

        assert(result);

        return result;
    }*/

    float* loadBOF(const char* filename, uint32** indices, uint32* vertexBufferSize, uint32* indexBufferSize)
    {
        float* result = 0;

        FILE* file = fopen(filename, "rb");
        if (file)
        {
            fread(vertexBufferSize, sizeof(uint32), 1, file);
            fread(indexBufferSize, sizeof(uint32), 1, file);

            result = new float[*vertexBufferSize];
            *indices = new uint32[*indexBufferSize];

            fread(result, *vertexBufferSize, 1, file);
            fread(*indices, *indexBufferSize, 1, file);

            fclose(file);
        }

        assert(result);

        return result;
    }

    uint8* loadImage(const char* filename, int32* x, int32* y, int32* n, bool flipY)
    {
        stbi_set_flip_vertically_on_load(flipY);
        uint8* data = stbi_load(filename, x, y, n, 4);
        assert(x != 0 && y != 0);
        return data;
    }

    void freeImageData(uint8* data)
    {
        stbi_image_free(data);
    }
}