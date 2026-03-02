#pragma once
#include "../GL4Context.h"

BEGIN_C_DECLARATIONS
crOpenGLContext crGLXCreateContext ( const crWindowHandle WindowHandle, const crRendererConfiguration NewConfiguration );
bool crGLXDeleteContext ( const crOpenGLContext Context );
bool crGLXMakeContextActive ( const crOpenGLContext Context, const crWindowHandle WindowHandle );
bool crGLXSwapWindowBuffer ( const crOpenGLContext Context, const crWindowHandle WindowHandle );
END_C_DECLARATIONS
