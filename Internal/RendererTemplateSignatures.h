#include "../Framebuffer.h"
#include "../Texture.h"
#include "../Shader.h"
#include "../ShaderBuffer.h"
#include "../RendererDefinitions.h"
#include "../RenderCommand.h"

// Framebuffer
DECLARE_INTERFACE_FUNCTION ( crFramebufferHandle, CreateFramebuffer, const crFramebufferDescriptor Descriptor );
DECLARE_INTERFACE_FUNCTION ( bool, DeleteFramebuffer, const crFramebufferHandle Handle );
DECLARE_INTERFACE_FUNCTION ( void, SetFramebufferClearColor, const crFramebufferHandle Handle, const vec4 ClearColor );
DECLARE_INTERFACE_FUNCTION ( void, SetFramebufferClearDepth, const crFramebufferHandle Handle, const float ClearDepth );
DECLARE_INTERFACE_FUNCTION ( void, BindDrawFramebuffer, const crFramebufferHandle Handle );
DECLARE_INTERFACE_FUNCTION ( void, ClearFramebuffer, const crFramebufferHandle Handle, const bool ShouldClearColorBuffer, const vec4 Color, const bool ShouldClearDepthBuffer, const float DepthClearValue, const bool ShouldClearStencilBuffer, const int StencilClearValue, const int StencilMask );
DECLARE_INTERFACE_FUNCTION ( void, ClearFramebufferWithDefaultValues, const crFramebufferHandle Handle );
DECLARE_INTERFACE_FUNCTION ( void, GetFramebufferSize, const crFramebufferHandle Handle, uvec2 *Dimensions );
DECLARE_INTERFACE_FUNCTION ( crTextureHandle, GetColorBufferFromFramebuffer, const crFramebufferHandle Handle, const size_t Index );
DECLARE_INTERFACE_FUNCTION ( crTextureHandle, GetDepthBufferFromFramebuffer, const crFramebufferHandle Handle );

// Shader
DECLARE_INTERFACE_FUNCTION ( crShaderHandle, CreateShader, crShaderCode NewCode );
DECLARE_INTERFACE_FUNCTION ( bool, DeleteShader, const crShaderHandle Handle );
DECLARE_INTERFACE_FUNCTION ( const crShaderInformation *, GetShaderInformation, const crShaderHandle Handle );

// Shader Buffer
DECLARE_INTERFACE_FUNCTION ( crShaderBufferHandle, CreateShaderBuffer, const crShaderBufferDescriptor CreationParameters );
DECLARE_INTERFACE_FUNCTION ( bool, DeleteShaderBuffer, const crShaderBufferHandle Handle );
DECLARE_INTERFACE_FUNCTION ( bool, ChangeShaderBufferContents, const crShaderBufferHandle Handle, const size_t Offset, const void *Data, const size_t DataSize );
DECLARE_INTERFACE_FUNCTION ( void *, MapShaderBuffer, const crShaderBufferHandle Handle, const crShaderBufferMapAccessType AccessType );
DECLARE_INTERFACE_FUNCTION ( bool, UnmapShaderBuffer, const crShaderBufferHandle Handle );

// Texture
DECLARE_INTERFACE_FUNCTION ( crTextureHandle, CreateTexture, const crTextureDescriptor Descriptor );
DECLARE_INTERFACE_FUNCTION ( bool, DeleteTexture, const crTextureHandle Handle );
DECLARE_INTERFACE_FUNCTION ( bool, Load2DTextureData, const crTextureHandle Handle, const crPixelFormat SourcePixelFormat, const void *data );
DECLARE_INTERFACE_FUNCTION ( bool, Update2DTextureRegion, const crTextureHandle Handle, const uvec2 LowerLeft, const uvec2 RegionDimensions, const crPixelFormat SourcePixelFormat, const void *Data );
DECLARE_INTERFACE_FUNCTION ( bool, LoadCubeMapTextureData, const crTextureHandle Handle, const crPixelFormat SourcePixelFormat, void *Data[6] );
DECLARE_INTERFACE_FUNCTION ( void, GetTextureDimensions, const crTextureHandle Handle, uvec2 *Dimensions );
DECLARE_INTERFACE_FUNCTION ( crPixelFormat, GetTextureFormat, const crTextureHandle Handle );

// Renderer
DECLARE_INTERFACE_FUNCTION ( bool, InitializeRenderer, const crRendererConfiguration NewConfiguration );
DECLARE_INTERFACE_FUNCTION ( bool, ShutdownRenderer, void );
DECLARE_INTERFACE_FUNCTION ( bool, IsExtensionAvailable, const char *Extension );
DECLARE_INTERFACE_FUNCTION ( bool, EnableDirectStateAccess, const bool NewState );
DECLARE_INTERFACE_FUNCTION ( bool, IsDirectStateAccessEnabled, void );
DECLARE_INTERFACE_FUNCTION ( bool, StartRenderToWindow, const crWindowHandle WindowHandle );
DECLARE_INTERFACE_FUNCTION ( bool, DisplayFramebuffer, const crFramebufferHandle FramebufferHandle, const crWindowHandle WindowHandle );
DECLARE_INTERFACE_FUNCTION ( bool, DisplayWindow, const crWindowHandle WindowHandle );
DECLARE_INTERFACE_FUNCTION ( bool, RunCommand, const crRenderCommand Command );
