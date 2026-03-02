#pragma once
#include "../WindowManager/WindowManager.h"
#include "../RendererDefinitions.h"

BEGIN_C_DECLARATIONS
typedef void *crOpenGLContext;

extern crOpenGLContext ( *crGL4CreateGLContext ) ( const crWindowHandle WindowHandle, const crRendererConfiguration NewConfiguration );
extern bool ( *crGL4DeleteGLContext ) ( const crOpenGLContext Context );
extern bool ( *crGL4MakeGLContextActive ) ( const crOpenGLContext Context, const crWindowHandle WindowHandle );
extern bool ( *crGL4SwapGLWindowBuffer ) ( const crOpenGLContext Context, const crWindowHandle WindowHandle );

bool crGL4InitializeGLContextFunctions ( const crWindowManagerBackend Backend ); // Setup function pointers to correct version of GL context creation
END_C_DECLARATIONS
