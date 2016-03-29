//
//  util.hpp
//  ggfx
//
//  Created by jon on 21/03/16.
//  Copyright © 2016 Joonatan Sörensen. All rights reserved.
//

#pragma once

#include "types.h"

namespace ggfx
{
    const uint8* loadFile(const char* filename);
    float32* loadBinaryOBJ(const char* filename, uint32** indices, uint32& positionsCount, uint32& uvsCount, uint32& normalsCount, uint32& indicesCount);
    uint8* loadImage(const char* filename, int32* x, int32* y, int32* n);
    void freeImageData(uint8* imageData);
}