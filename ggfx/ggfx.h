//
//  ggfx.h
//  ggfx
//
//  Created by jon on 19/03/16.
//  Copyright © 2016 Joonatan Sörensen. All rights reserved.
//

#pragma once

#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>

#include "types.h"

namespace ggfx
{
    uint32 createShaderProgram(uint32 type, const uint8* source);
    uint32 createShaderProgramFromFile(const char* filename, uint32 type);
    uint32 createProgramPipeline(uint32 vertexProgram, uint32 fragmentProgram);
}