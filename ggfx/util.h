//
//  util.hpp
//  ggfx
//
//  Created by jon on 21/03/16.
//  Copyright © 2016 Joonatan Sörensen. All rights reserved.
//

#pragma once

#include "types.h"

const uint8* loadFile(const char* filename);

uint8* loadImage(const char* filename, int32* x, int32* y, int32* n);

void freeImageData(uint8* imageData);
