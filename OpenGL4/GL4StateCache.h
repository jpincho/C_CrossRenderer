#pragma once
#include "../RenderStateDefinitions.h"

void crGL4SetDefaultViewportSize ( const uvec2 NewSize );
bool crGL4ApplyState ( const struct crRenderState *NewState );
bool crGL4ApplyStateWithCache ( const struct crRenderState *NewState );
bool crGL4ApplyStateWithoutCache ( const struct crRenderState *NewState );
void crGL4Invalidate ( void );
struct crRenderState crGL4GetCurrentState ( void );
void crGL4ConfigureCulling ( const struct crCullingSettings *NewState );
void crGL4ConfigureBlending ( const struct crBlendSettings *NewSettings );
void crGL4ConfigureStencil ( const struct crStencilBufferSettings *NewSettings );
void crGL4ConfigureScissor ( const struct crScissorSettings *NewSettings );
void crGL4ConfigureViewport ( const struct crViewportSettings *NewSettings );
void crGL4ConfigureDepthTest ( const struct crDepthTestSettings *NewSettings );
void crGL4ConfigureFramebuffer ( const crFramebufferHandle NewFramebuffer );
void crGL4ConfigurePolygonMode ( const struct crPolygonModeSettings *NewSettings );
