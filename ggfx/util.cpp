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

    inline bool operator <(const MeshIndex& a, const MeshIndex& b)
    {
        return a.indices[0] < b.indices[0];
        return a.indices[1] < b.indices[1];
        return a.indices[2] < b.indices[2];
        return false;
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
    float32* loadBinaryOBJ(
        const char* filename, 
        uint32** indices, 
        uint32& vpSizeInBytesOut, 
        uint32& vtSizeInBytesOut, 
        uint32& vnSizeInBytesOut,
        uint32& fSizeinBytesOut)
    {
        float32* result = 0;

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

            uint32 vpSize = vpSizeInBytes / sizeof(float32);
            uint32 vtSize = vtSizeInBytes / sizeof(float32);
            uint32 vnSize = vnSizeInBytes / sizeof(float32);
            uint32 fSize = fSizeInBytes / sizeof(uint32);

            float32* positions = new float32[vpSize];
            float32* uvs = new float32[vtSize];
            float32* normals = new float32[vnSize];
            uint32* faceElements = new uint32[fSize];

            fread(positions, vpSizeInBytes, 1, file);
            fread(uvs, vtSizeInBytes, 1, file);
            fread(normals, vnSizeInBytes, 1, file);
            fread(faceElements, fSizeInBytes, 1, file);

            /*for (uint32 i = 0; i < vnSizeInBytes / sizeof(float32) / 3; i+=3)
            {
                Log::print("vn: %f, %f, %f\n", normals[i], normals[i + 1], normals[i + 2]);
            }*/

            result = new float32[vpSize + vtSize + vnSize];
            *indices = new uint32[fSizeInBytes / sizeof(uint32) / attributeCount];
            
            uint32 indicesCount = fSizeInBytes / sizeof(uint32) / attributeCount;
            uint32 indicesIndex = 0;
            uint32 stride = 8;
            std::map<MeshIndex, uint32> uniqueIndices;
            for (uint32 i = 0, k = 0; k < indicesCount; i+=3, k++)
            {
                MeshIndex m = { { faceElements[i], faceElements[i + 1], faceElements[i + 2] } };

                uint32 count = uniqueIndices.count(m);

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

            vpSizeInBytesOut = indicesIndex * stride * sizeof(float32);
            vtSizeInBytesOut = vtSizeInBytes;
            vnSizeInBytesOut = vnSizeInBytes;
            fSizeinBytesOut = indicesCount * sizeof(float32);

            fclose(file);
        }

        assert(result);

        return result;
    }

    float32* loadBOF(const char* filename, uint32** indices, uint32* vertexBufferSize, uint32* indexBufferSize)
    {
        float32* result = 0;

        FILE* file = fopen(filename, "rb");
        if (file)
        {
            fread(vertexBufferSize, sizeof(uint32), 1, file);
            fread(indexBufferSize, sizeof(uint32), 1, file);

            result = new float32[*vertexBufferSize];
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
        assert(data);
        return data;
    }

    void freeImageData(uint8* data)
    {
        stbi_image_free(data);
    }
}