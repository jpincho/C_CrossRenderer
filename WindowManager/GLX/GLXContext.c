#include "GLXContext.h"
#include <glad/glad_GLX.h>

crOpenGLContext crGLXCreateContext ( const crWindowHandle WindowHandle, const crRendererConfiguration NewConfiguration )
	{
	return NULL;
	}

bool crGLXDeleteContext ( const crOpenGLContext Context )
	{
	return true;
	}

bool crGLXMakeContextActive ( const crOpenGLContext Context, const crWindowHandle WindowHandle )
	{
        glXMakeContextCurrent ( ContextInfo.MyDisplay, MyGLXWindow, MyGLXWindow, ContextInfo.MyGLContext );

	return true;
	}

void crGLXSwapWindowBuffer (const crOpenGLContext Context, const crWindowHandle WindowHandle )
	{
	}
