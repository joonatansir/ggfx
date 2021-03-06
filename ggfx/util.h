//
//  util.hpp
//  ggfx
//
//  Created by jon on 21/03/16.
//  Copyright © 2016 Joonatan Sörensen. All rights reserved.
//

#pragma once

#include <assert.h>
#include <string>

#include "types.h"

#define COUNT(x) (sizeof(x)/sizeof(x[0]))

namespace ggfx
{
    const uint8* loadFile(const std::string& filename);
    float* loadBinaryOBJ(const char* filename, uint32** indices, uint32& positionsCount, uint32& uvsCount, uint32& normalsCount, uint32& indicesCount);
    float* loadBOF(const std::string& filename, uint32** indices, uint32* vertexBufferSize, uint32* indexBufferSize);
    uint8* loadImage(const std::string& filename, int32* x, int32* y, int32* n, bool flipY = true);
    void freeImageData(uint8* imageData);
}