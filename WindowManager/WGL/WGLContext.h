#pragma once
#include "../GL4Context.h"

BEGIN_C_DECLARATIONS
crOpenGLContext crWGLCreateContext ( const crWindowHandle WindowHandle, const crRendererConfiguration NewConfiguration );
bool crWGLDeleteContext ( void );
bool crWGLMakeContextActive ( const crWindowHandle WindowHandle );
void crWGLSwapWindowBuffer ( const crWindowHandle WindowHandle );
END_C_DECLARATIONS
