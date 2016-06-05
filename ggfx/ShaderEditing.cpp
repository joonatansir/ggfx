#if defined(_DEBUG)

#include <windows.h>
#include <atlbase.h>

#include "ShaderEditing.h"
#include "Log.h"

using namespace ggfx;

void checkForShaderUpdate(Shader& shader)
{
    USES_CONVERSION;
    HANDLE file = CreateFile(
        A2W(shader.info.filename.c_str()),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (file != INVALID_HANDLE_VALUE)
    {
        FILETIME filetime;
        if (GetFileTime(file, NULL, NULL, &filetime))
        {
            uint64 lastModified = filetime.dwHighDateTime;
            lastModified = (lastModified << 32) | filetime.dwLowDateTime;
            
            if (shader.info.lastModified != lastModified)
            {
                shader.info.lastModified = lastModified;
                shader.recompile();
            }
        }
        CloseHandle(file);
    }
}

#endif