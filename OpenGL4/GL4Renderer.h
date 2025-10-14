#pragma once
#include "../RendererDefinitions.h"
#include "../RenderCommand.h"
#include "GL4Framebuffer.h"
#include "GL4Shader.h"
#include "GL4ShaderBuffer.h"
#include "GL4Texture.h"

bool crGL4InitializeRenderer ( const struct crRendererConfiguration NewConfiguration );
bool crGL4ShutdownRenderer ( void );
bool crGL4IsExtensionAvailable ( const char *Extension );
bool crGL4EnableDirectStateAccess ( const bool NewState );
bool crGL4IsDirectStateAccessEnabled ( void );
bool crGL4StartRenderToWindow ( const crWindowHandle WindowHandle );
bool crGL4DisplayFramebuffer ( const crFramebufferHandle FramebufferHandle, const crWindowHandle WindowHandle );
bool crGL4DisplayWindow ( const crWindowHandle WindowHandle );
bool crGL4RunCommand ( const struct crRenderCommand Command );

#if 0
bool DisplayFramebuffer ( const FramebufferHandle &Handle, const RenderWindowHandle &WindowHandle );
bool StartRenderToWindow ( const RenderWindowHandle &WindowHandle );
bool DisplayWindow ( const RenderWindowHandle &WindowHandle );

bool RunCommand ( const RenderCommand &Command );

bool IsExtensionAvailable ( const char *Extension );
bool EnableDirectStateAccess ( const bool NewState );
bool IsDirectStateAccessEnabled ( void );
#endif
