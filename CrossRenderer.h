#pragma once
#include <Platform/defines.h>
#include "WindowManagerDefinitions.h"
#include "Framebuffer.h"
#include "RenderStateDefinitions.h"
#include "ShaderBuffer.h"
#include "Texture.h"
#include "RenderCommand.h"
#include "Shader.h"
#include "Internal/CrossRendererFunctionPointers.h"

BEGIN_C_DECLARATIONS
bool crInitialize ( const struct crRendererConfiguration NewConfiguration );
bool crUpdate ( void );
bool crShutdown ( void );

crWindowHandle crCreateNewWindow ( const struct crRenderWindowDescriptor Descriptor );
bool crDestroyWindow ( const crWindowHandle WindowHandle );
bool crUpdateWindows ( const bool Wait );
bool crSetWindowPosition ( const crWindowHandle WindowHandle, const ivec2 Position );
bool crGetWindowPosition ( const crWindowHandle WindowHandle, ivec2 *Position );
bool crSetWindowDimensions ( const crWindowHandle WindowHandle, const uvec2 Dimensions );
bool crGetWindowDimensions ( const crWindowHandle WindowHandle, uvec2 *Dimensions );
bool crGetWindowClientAreaDimensions ( const crWindowHandle WindowHandle, uvec2 *Dimensions );
bool crSetWindowTitle ( const crWindowHandle WindowHandle, const char *Title );
const char *crGetWindowTitle ( const crWindowHandle WindowHandle );
bool crActivateWindow ( const crWindowHandle WindowHandle );

void crSetWindowManagerCallbacks ( crWindowManagerCallbacks NewCallbacks );
crWindowManagerCallbacks crGetWindowManagerCallbacks ( void );

crWindowHandle crGetMainWindowHandle ( void );

void SetRenderStateToDefault ( struct crRenderState *State );
END_C_DECLARATIONS
