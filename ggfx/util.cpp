//
//  util.cpp
//  ggfx
//
//  Created by jon on 21/03/16.
//  Copyright © 2016 Joonatan Sörensen. All rights reserved.
//

#include <cstdio>
#include <cassert>

#include "util.h"
#include "types.h"

const char* loadFile(const char* fileName)
{
    char* result = 0;
    
    FILE* file = fopen(fileName, "r");
    if(file)
    {
        fseek(file, 0, SEEK_END);
        uint64 fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        result = new char[fileSize];
        fread(result, fileSize, 1, file);
        
        fclose(file);
    }
    
    assert(result);
    
    return result;
}