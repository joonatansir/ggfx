//
//  util.cpp
//  ggfx
//
//  Created by jon on 21/03/16.
//  Copyright © 2016 Joonatan Sörensen. All rights reserved.
//

#include <stdio.h>
#include <assert.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "util.h"
#include "types.h"

namespace ggfx
{
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

            glm::tvec3<float32>* positions = new glm::tvec3<float32>[vpSize / 3];
            glm::tvec2<float32>* uvs = new glm::tvec2<float32>[vtSize / 2];
            glm::tvec3<float32>* normals = new glm::tvec3<float32>[vnSize / 3];
            uint32* faceElements = new uint32[fSize];

            fread(positions, vpSizeInBytes, 1, file);
            fread(uvs, vtSizeInBytes, 1, file);
            fread(normals, vnSizeInBytes, 1, file);
            fread(faceElements, fSizeInBytes, 1, file);

            result = new float32[vpSize + vtSize + vnSize];
            *indices = new uint32[fSizeInBytes / sizeof(uint32) / attributeCount];
            
            uint32 faceElementsCount = fSizeInBytes / sizeof(uint32);
            uint32 indicesIndex = 0;
            uint32 stride = 8;
            for (uint32 i = 0; i < faceElementsCount; i+=3)
            {
                (*indices)[indicesIndex++] = faceElements[i];

                result[faceElements[i] * stride + 0] = positions[faceElements[i]].x;
                result[faceElements[i] * stride + 1] = positions[faceElements[i]].y;
                result[faceElements[i] * stride + 2] = positions[faceElements[i]].z;

                /*result[faceElements[i + 2] * stride + 3] = normals[faceElements[i + 2]].x;
                result[faceElements[i + 2] * stride + 4] = normals[faceElements[i + 2]].y;
                result[faceElements[i + 2] * stride + 5] = normals[faceElements[i + 2]].z;

                result[faceElements[i + 1] * stride + 6] = uvs[faceElements[i + 1]].x;
                result[faceElements[i + 1] * stride + 7] = uvs[faceElements[i + 1]].y;*/
            }

            vpSizeInBytesOut = vpSizeInBytes;
            vtSizeInBytesOut = vtSizeInBytes;
            vnSizeInBytesOut = vnSizeInBytes;
            fSizeinBytesOut = fSizeInBytes / attributeCount;
        }

        assert(result);

        return result;
    }

    uint8* loadImage(const char* filename, int32* x, int32* y, int32* n)
    {
        stbi_set_flip_vertically_on_load(true);
        return stbi_load(filename, x, y, n, 4);
    }

    void freeImageData(uint8* data)
    {
        stbi_image_free(data);
    }
}