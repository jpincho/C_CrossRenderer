#pragma once
#include "../GL4Context.h"

BEGIN_C_DECLARATIONS
crOpenGLContext crWGLCreateContext ( const crWindowHandle WindowHandle, const crRendererConfiguration NewConfiguration );
bool crWGLDeleteContext ( const crOpenGLContext Context );
bool crWGLMakeContextActive ( const crOpenGLContext Context, const crWindowHandle WindowHandle );
bool crWGLSwapWindowBuffer ( const crOpenGLContext Context, const crWindowHandle WindowHandle );
END_C_DECLARATIONS
