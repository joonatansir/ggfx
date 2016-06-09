#include "Log.h"

#include "GPUTimer.h"

using namespace ggfx;

GPUTimer::~GPUTimer()
{
    glDeleteQueries(1, &queryBegin);
    glDeleteQueries(1, &queryEnd);
}

void GPUTimer::create()
{
    glGenQueries(1, &queryBegin);
    glGenQueries(1, &queryEnd);
}

void GPUTimer::begin()
{
    if (queryBegin == 0 || queryEnd == 0)
        create();

    glQueryCounter(queryBegin, GL_TIMESTAMP);
}

void GPUTimer::end()
{
    glQueryCounter(queryEnd, GL_TIMESTAMP);

    int64 beginTimestamp;
    int64 endTimestamp;
    glGetQueryObjecti64v(queryBegin, GL_QUERY_RESULT, &beginTimestamp);
    glGetQueryObjecti64v(queryEnd, GL_QUERY_RESULT, &endTimestamp);
    timeElapsed = (endTimestamp - beginTimestamp) / 1000000.0f;
}

float GPUTimer::getTime()
{
    return timeElapsed;
}