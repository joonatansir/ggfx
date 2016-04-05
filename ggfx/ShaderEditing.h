#pragma once

#include "Log.h"

#if defined(_DEBUG)
    #define LIVE_EDIT(x) headerStatic(x)
#else
    #define LIVE_EDIT(x)
#endif

void headerStatic(const char* thing);