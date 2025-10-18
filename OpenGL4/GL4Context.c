#include <Platform/Platform.h>
#include "GL4Context.h"
#if defined PLATFORM_WINDOWS
#include "WGL/WGLContext.h"
#endif
crOpenGLContext(*crGL4CreateGLContext) (const crWindowHandle WindowHandle, const crRendererConfiguration NewConfiguration) = NULL;
bool (*crGL4DeleteGLContext) (void) = NULL;
bool (*crGL4MakeGLContextActive) (const crWindowHandle WindowHandle) = NULL;
void (*crGL4SwapGLWindowBuffer) (const crWindowHandle WindowHandle) = NULL;

bool crGL4InitializeGLContextFunctions(void)
    {
#if defined PLATFORM_WINDOWS
    crGL4CreateGLContext = crWGLCreateContext;
    crGL4DeleteGLContext = crWGLDeleteContext;
    crGL4MakeGLContextActive = crWGLMakeContextActive;
    crGL4SwapGLWindowBuffer = crWGLSwapWindowBuffer;
#endif
    return true;
    }