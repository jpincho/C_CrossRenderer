#include "CrossRenderer.h"
#include "CrossRendererConfig.h"
#include "WindowManager/Internal/WindowManagerInternal.h"
#include "Internal/CrossRendererFunctionPointers.h"
#include <memory.h>
#include <Platform/Logger.h>

// Interface creation

#define DECLARE_INTERFACE_FUNCTION(return,name,...)\
return (*cr##name) ( __VA_ARGS__ ) = 0

#include "Internal/RendererTemplateSignatures.h"

#undef DECLARE_INTERFACE_FUNCTION

#if defined ( CROSS_RENDERER_OPENGL_CORE_SUPPORT)
#include "OpenGL4/GL4Renderer.h"
#endif

bool SetupFunctionPointers ( const crRendererBackend Backend )
	{
	switch ( Backend )
		{
#if defined ( CROSS_RENDERER_OPENGL_CORE_SUPPORT)
#define DECLARE_INTERFACE_FUNCTION(return,name,...) cr##name = crGL4##name
		case OpenGLCore:
#include "Internal/RendererTemplateSignatures.h"
			break;
#undef DECLARE_INTERFACE_FUNCTION
#endif
		default:
			LOG_ERROR ( "Renderer backend is not built" );
			return false;
		}
	return true;
	}

void crSetConfigurationToDefault(crRendererConfiguration *Configuration)
{
	memset(Configuration, 0, sizeof(crRendererConfiguration));

	Configuration->InitialWindowDescriptor.Fullscreen = false;
	Configuration->InitialWindowDescriptor.Position.x = Configuration->InitialWindowDescriptor.Position.y = 0;
	Configuration->InitialWindowDescriptor.Resizable = true;
	Configuration->InitialWindowDescriptor.Size.x = 1920;
	Configuration->InitialWindowDescriptor.Size.y = 1080;
	Configuration->InitialWindowDescriptor.SupportOpenGL = true;
	Configuration->InitialWindowDescriptor.Title = "CrossRenderer Window";
	 
	Configuration->RedBits = 8;
	Configuration->GreenBits = 8;
	Configuration->BlueBits = 8;
	Configuration->AlphaBits = 8;
	Configuration->DepthBits = 32;
	Configuration->StencilBits = 0;
	Configuration->DesiredRendererBackend = OpenGLCore;
	Configuration->DesiredWindowManagerBackend = WindowManagerBackend_Windows;
	Configuration->VSyncEnabled = true;
}

bool crInitialize ( const crRendererConfiguration NewConfiguration )
	{
	if ( SetupFunctionPointers ( NewConfiguration.DesiredRendererBackend ) == false )
		return false;
	if (SetupWindowManagerFunctionPointers(NewConfiguration.DesiredWindowManagerBackend) == false)
		return false;
	if ( crInitializeRenderer ( NewConfiguration ) == false )
		return false;
        MainWindowHandle = ( crWindowHandle ) PointerList_GetNodeData ( WindowList.first );
	return true;
	}

bool crUpdate ( void )
	{
	return true;
	}

bool crShutdown ( void )
	{
	if ( MainWindowHandle )
		{
		crDestroyWindow ( MainWindowHandle );
		MainWindowHandle = NULL;
		}
	return true;
	}

crWindowHandle crGetMainWindowHandle ( void )
	{
	return MainWindowHandle;
	}

void crSetWindowManagerCallbacks ( crWindowManagerCallbacks NewCallbacks )
	{
	WindowManagerCallbacks = NewCallbacks;
	}

crWindowManagerCallbacks crGetWindowManagerCallbacks ( void )
	{
	return WindowManagerCallbacks;
	}

void crSetRenderStateToDefault ( crRenderState *State )
	{
	memset ( State, 0, sizeof ( crRenderState ) );

	State->Blending.Source = crBlendMode_SourceAlpha;
	State->Blending.Destination = crBlendMode_OneMinusSourceAlpha;
	State->Blending.Enabled = true;

	State->Culling.Enabled = false;
	State->Culling.Mode = crCullingMode_Back;
	State->Culling.Winding = crCullingFaceWinding_CounterClockwise;

	State->DepthTest.Enabled = false;
	State->DepthTest.Mode = crDepthTestMode_LessOrEqual;

	State->PolygonMode.State = crPolygonMode_Fill;

	State->Scissor.Enabled = false;

	State->Stencil.Enabled = false;
	State->Stencil.Mask = ( unsigned ) - 1;
	State->Stencil.Function = crStencilFunction_Always;
	State->Stencil.FunctionReference = State->Stencil.FunctionMask = ( unsigned ) - 1;
	State->Stencil.OnFail = State->Stencil.OnFailZ = State->Stencil.OnPassZ = crStencilFailAction_Keep;

	State->Viewport.Enabled = false;
	}
