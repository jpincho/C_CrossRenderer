#pragma once
#include "../Framebuffer.h"
#include "../Texture.h"

BEGIN_C_DECLARATIONS
crFramebufferHandle crGL4CreateFramebuffer ( const crFramebufferDescriptor Descriptor );
bool crGL4DeleteFramebuffer ( const crFramebufferHandle Handle );
void crGL4SetFramebufferClearColor ( const crFramebufferHandle Handle, const vec4 ClearColor );
void crGL4SetFramebufferClearDepth ( const crFramebufferHandle Handle, const float ClearDepth );
void crGL4BindDrawFramebuffer ( const crFramebufferHandle Handle );
void crGL4ClearFramebuffer ( const crFramebufferHandle Handle, const bool ShouldClearColorBuffer, const vec4 Color, const bool ShouldClearDepthBuffer, const float DepthClearValue, const bool ShouldClearStencilBuffer, const int StencilClearValue, const int StencilMask );
void crGL4ClearFramebufferWithDefaultValues ( const crFramebufferHandle Handle );
void crGL4GetFramebufferSize ( const crFramebufferHandle Handle, uvec2 *Dimensions );
crTextureHandle crGL4GetColorBufferFromFramebuffer ( const crFramebufferHandle Handle, const size_t Index );
crTextureHandle crGL4GetDepthBufferFromFramebuffer ( const crFramebufferHandle Handle );
END_C_DECLARATIONS
