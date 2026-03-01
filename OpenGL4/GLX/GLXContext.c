#include "GLXContext.h"
#include "../../WindowManager/X11/X11Internal.h"
#include <glad/glad_glx.h>
#include <Platform/Logger.h>

struct InternalGLXContextData
        {
        GLXContext Context;
        Display *DisplayHandle;
        };

crOpenGLContext crGLXCreateContext ( const crWindowHandle WindowHandle, const crRendererConfiguration NewConfiguration )
	{
        struct InternalX11WindowData *WindowData = GetInternalX11WindowDataFromcrWindowHandle ( WindowHandle );
        if ( WindowData == NULL )
                return NULL;

        GLXContext NewGLXContext = NULL;
        if ( !gladLoadGLX ( WindowData->DisplayHandle, WindowData->ScreenID ) )
                return NULL;

        // Create the context
        int Attributes[] =
                {
                GLX_RENDER_TYPE, GLX_RGBA_BIT,
                GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
                GLX_DOUBLEBUFFER, true,
                GLX_RED_SIZE, NewConfiguration.RedBits,
                GLX_GREEN_SIZE, NewConfiguration.GreenBits,
                GLX_BLUE_SIZE, NewConfiguration.BlueBits,
                None
                };

        int FramebufferConfigCount = 0;
        GLXFBConfig *FramebufferConfigs = glXChooseFBConfig ( WindowData->DisplayHandle,
                                          WindowData->ScreenID,
                                          Attributes,
                                          &FramebufferConfigCount );
        if ( FramebufferConfigs == NULL )
                {
                LOG_ERROR ( "glXChooseFBConfig failed" );
                goto OnError;
                }

        static int ContextAttributes[] =
                {
                GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
                GLX_CONTEXT_MINOR_VERSION_ARB, 2,
                GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
                None
                };

        NewGLXContext = glXCreateContextAttribsARB ( WindowData->DisplayHandle, FramebufferConfigs[0], NULL, true, ContextAttributes );
        if ( !NewGLXContext )
                {
                LOG_ERROR ( "glXCreateContextAttribsARB failed" );
                goto OnError;
                }

        struct InternalGLXContextData *NewContextData = calloc ( 1, sizeof ( struct InternalGLXContextData ) );
        if ( !NewContextData )
                return NULL;
        NewContextData->Context = NewGLXContext;
        NewContextData->DisplayHandle = WindowData->DisplayHandle;

        glXMakeContextCurrent ( NewContextData->DisplayHandle, WindowData->X11WindowHandle, WindowData->X11WindowHandle, NewContextData->Context );
        return NewContextData;
OnError:
        if ( FramebufferConfigs )
                XFree ( FramebufferConfigs );
        if ( NewGLXContext )
                glXDestroyContext ( WindowData->DisplayHandle, NewGLXContext );

        return NULL;
	}

bool crGLXDeleteContext ( const crOpenGLContext Context )
	{
        struct InternalGLXContextData *ContextData = ( struct InternalGLXContextData* ) Context;
        glXDestroyContext ( ContextData->DisplayHandle, ContextData->Context );
        free ( ContextData );
	return true;
	}

bool crGLXMakeContextActive ( const crOpenGLContext Context, const crWindowHandle WindowHandle )
	{
        struct InternalGLXContextData *ContextData = ( struct InternalGLXContextData* ) Context;
        struct InternalX11WindowData *WindowData = GetInternalX11WindowDataFromcrWindowHandle ( WindowHandle );
        if ( WindowData == NULL )
                return false;
        glXMakeContextCurrent ( ContextData->DisplayHandle, WindowData->X11WindowHandle, WindowData->X11WindowHandle, ContextData->Context );

	return true;
	}

bool crGLXSwapWindowBuffer ( const crOpenGLContext Context, const crWindowHandle WindowHandle )
	{
        struct InternalGLXContextData *ContextData = ( struct InternalGLXContextData* ) Context;
        struct InternalX11WindowData *WindowData = GetInternalX11WindowDataFromcrWindowHandle ( WindowHandle );
        if ( WindowData == NULL )
                return false;
        glXSwapBuffers ( ContextData->DisplayHandle, WindowData->X11WindowHandle );
        return true;
        }
