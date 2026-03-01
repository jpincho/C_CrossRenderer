#include <Platform/Platform.h>
#include "GL4Context.h"
#if defined PLATFORM_WINDOWS
#include "WGL/WGLContext.h"
#endif

#if defined PLATFORM_LINUX
#include "GLX/GLXContext.h"
#endif

crOpenGLContext ( *crGL4CreateGLContext ) ( const crWindowHandle WindowHandle, const crRendererConfiguration NewConfiguration ) = NULL;
bool ( *crGL4DeleteGLContext ) ( const crOpenGLContext Context ) = NULL;
bool ( *crGL4MakeGLContextActive ) ( const crOpenGLContext Context, const crWindowHandle WindowHandle ) = NULL;
bool ( *crGL4SwapGLWindowBuffer ) ( const crOpenGLContext Context, const crWindowHandle WindowHandle ) = NULL;

bool crGL4InitializeGLContextFunctions ( const crWindowManagerBackend Backend )
	{
        switch ( Backend )
                {
#if defined PLATFORM_WINDOWS
                case WindowManagerBackend_Windows:
                        {
                        crGL4CreateGLContext = crWGLCreateContext;
                        crGL4DeleteGLContext = crWGLDeleteContext;
                        crGL4MakeGLContextActive = crWGLMakeContextActive;
                        crGL4SwapGLWindowBuffer = crWGLSwapWindowBuffer;
                        break;
                        }
#endif
#if defined PLATFORM_LINUX
                case WindowManagerBackend_X11:
                        {
                        crGL4CreateGLContext = crGLXCreateContext;
                        crGL4DeleteGLContext = crGLXDeleteContext;
                        crGL4MakeGLContextActive = crGLXMakeContextActive;
                        crGL4SwapGLWindowBuffer = crGLXSwapWindowBuffer;
                        break;
                        }
#endif
                default:
                        return false;
                }
	return true;
	}
