#pragma once
#include "../RenderStateDefinitions.h"

BEGIN_C_DECLARATIONS
void crGL4SetDefaultViewportSize ( const uvec2 NewSize );
bool crGL4ApplyState ( const crRenderState *NewState );
bool crGL4ApplyStateWithCache ( const crRenderState *NewState );
bool crGL4ApplyStateWithoutCache ( const crRenderState *NewState );
void crGL4Invalidate ( void );
crRenderState crGL4GetCurrentState ( void );
void crGL4ConfigureCulling ( const crCullingSettings *NewState );
void crGL4ConfigureBlending ( const crBlendSettings *NewSettings );
void crGL4ConfigureStencil ( const crStencilBufferSettings *NewSettings );
void crGL4ConfigureScissor ( const crScissorSettings *NewSettings );
void crGL4ConfigureViewport ( const crViewportSettings *NewSettings );
void crGL4ConfigureDepthTest ( const crDepthTestSettings *NewSettings );
void crGL4ConfigureFramebuffer ( const crFramebufferHandle NewFramebuffer );
void crGL4ConfigurePolygonMode ( const crPolygonModeSettings *NewSettings );
END_C_DECLARATIONS
