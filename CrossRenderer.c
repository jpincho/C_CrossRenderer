#include "CrossRenderer.h"
#include "CrossRendererConfig.h"
#include "Internal/WindowManagerInternal.h"
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

bool SetupFunctionPointers(const crRendererBackend Backend)
    {
    switch (Backend)
        {
#if defined ( CROSS_RENDERER_OPENGL_CORE_SUPPORT)
#define DECLARE_INTERFACE_FUNCTION(return,name,...) cr##name = crGL4##name
        case OpenGLCore:
#include "Internal/RendererTemplateSignatures.h"
            break;
#undef DECLARE_INTERFACE_FUNCTION
#endif
        default:
            LOG_ERROR("Renderer backend is not built");
            return false;
        }
    return true;
    }

bool crInitialize(const crRendererConfiguration NewConfiguration)
    {
    if (SetupFunctionPointers(NewConfiguration.DesiredRendererBackend) == false)
        return false;

    if (crInitializeRenderer(NewConfiguration) == false)
        return false;
    MainWindowHandle = pointer_list_get_node_data(WindowList.first);
    return true;
    }

bool crUpdate(void)
    {
    return true;
    }

bool crShutdown(void)
    {
    if (MainWindowHandle)
        {
        crDestroyWindow(MainWindowHandle);
        MainWindowHandle = NULL;
        }
    return true;
    }

crWindowHandle crGetMainWindowHandle(void)
    {
    return MainWindowHandle;
    }

void crSetWindowManagerCallbacks(crWindowManagerCallbacks NewCallbacks)
    {
    WindowManagerCallbacks = NewCallbacks;
    }

crWindowManagerCallbacks crGetWindowManagerCallbacks(void)
    {
    return WindowManagerCallbacks;
    }

void SetRenderStateToDefault(crRenderState *State)
    {
    memset(State, 0, sizeof(crRenderState));

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
    State->Stencil.Mask = (unsigned)-1;
    State->Stencil.Function = crStencilFunction_Always;
    State->Stencil.FunctionReference = State->Stencil.FunctionMask = (unsigned)-1;
    State->Stencil.OnFail = State->Stencil.OnFailZ = State->Stencil.OnPassZ = crStencilFailAction_Keep;

    State->Viewport.Enabled = false;
    }
