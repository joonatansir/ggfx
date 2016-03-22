//
//  util.cpp
//  ggfx
//
//  Created by jon on 21/03/16.
//  Copyright © 2016 Joonatan Sörensen. All rights reserved.
//

#include <cstdio>
#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "util.h"
#include "types.h"

namespace ggfx
{
    const uint8* loadFile(const char* filename)
    {
        uint8* result = 0;
        
        FILE* file = fopen(filename, "r");
        if(file)
        {
            fseek(file, 0, SEEK_END);
            uint64 fileSize = ftell(file);
            fseek(file, 0, SEEK_SET);
            
            result = new uint8[fileSize + 1];
            
            fread(result, fileSize, 1, file);
            result[fileSize] = 0;
            
            fclose(file);
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