#pragma once
#include <Platform/defines.h>
#include "WindowManager/WindowManager.h"
#include "Framebuffer.h"
#include "RenderStateDefinitions.h"
#include "ShaderBuffer.h"
#include "Texture.h"
#include "RenderCommand.h"
#include "Shader.h"
#include "Internal/CrossRendererFunctionPointers.h"

BEGIN_C_DECLARATIONS
void crSetConfigurationToDefault ( crRendererConfiguration *Configuration );
bool crInitialize ( const crRendererConfiguration NewConfiguration );
bool crUpdate ( void );
bool crShutdown ( void );

void crSetWindowManagerCallbacks ( crWindowManagerCallbacks NewCallbacks );
crWindowManagerCallbacks crGetWindowManagerCallbacks ( void );

crWindowHandle crGetMainWindowHandle ( void );

void crSetRenderStateToDefault ( crRenderState *State );
END_C_DECLARATIONS
