#pragma once
#include "../WindowManagerDefinitions.h"
#include "../RendererDefinitions.h"

BEGIN_C_DECLARATIONS
typedef void *crOpenGLContext;

extern crOpenGLContext ( *crGL4CreateGLContext ) ( const crWindowHandle WindowHandle, const crRendererConfiguration NewConfiguration );
extern bool ( *crGL4DeleteGLContext ) ( void );
extern bool ( *crGL4MakeGLContextActive ) ( const crWindowHandle WindowHandle );
extern void ( *crGL4SwapGLWindowBuffer ) ( const crWindowHandle WindowHandle );

bool crGL4InitializeGLContextFunctions ( void ); // Setup function pointers to correct version of GL context creation
END_C_DECLARATIONS
