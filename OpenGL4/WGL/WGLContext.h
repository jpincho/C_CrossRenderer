#pragma once
#include "../GL4Context.h"

crOpenGLContext crWGLCreateContext ( const crWindowHandle WindowHandle, const crRendererConfiguration NewConfiguration );
bool crWGLDeleteContext ( void );
bool crWGLMakeContextActive ( const crWindowHandle WindowHandle );
void crWGLSwapWindowBuffer ( const crWindowHandle WindowHandle );
