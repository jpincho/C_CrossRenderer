#include <CrossRenderer.h>
#include <Platform/Logger.h>
#include "../CommonLoaders.h"

#define _USE_MATH_DEFINES
#include <math.h>

#if defined (PLATFORM_COMPILER_GCC)
#include <stddef.h>
#pragma GCC diagnostic ignored "-Wmissing-braces"
#endif

static bool ShouldQuit = false;
bool LoadFileContents ( const char *Filename, char **Contents );

void WindowClosed_Callback ( const crWindowHandle Handle )
	{
	UNUSED ( Handle );
	ShouldQuit = true;
	}

int main ( void )
	{
	crRendererConfiguration Configuration;
	crSetConfigurationToDefault(&Configuration);
	Configuration.InitialWindowDescriptor.Title = "CrossRenderer - textured quad test";
	if ( crInitialize ( Configuration ) == false )
		return -1;

	crWindowManagerCallbacks Callbacks = { 0 };
	Callbacks.WindowClosed = WindowClosed_Callback;
	crSetWindowManagerCallbacks ( Callbacks );

	struct Vertex
		{
		vec3 Position;
		vec2 TexCoord;
		};
	struct Vertex Vertices[4] =
		{
			{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
			{{ 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
			{{-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f}},
			{{ 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f}}
		};

	uvec2 WindowSize;
	crGetWindowDimensions ( crGetMainWindowHandle(), &WindowSize );
	mat4 ProjectionMatrix;
	math_mat4_set_perspective_matrix ( &ProjectionMatrix, ( float ) M_PI_2, ( float ) WindowSize.x / ( float ) WindowSize.y, 0.1f, 10.0f );
	mat4 ModelMatrix;
	math_mat4_set_identity_matrix ( &ModelMatrix );
	mat4 ViewMatrix;
	math_mat4_set_view_matrix ( &ViewMatrix, ( vec3 )
		{
		0.0f, 0.0f, 2.0f
		}, ( vec3 )
		{
		0.0f, 0.0f, 0.0f
		}, ( vec3 )
		{
		0, 1.0f, 0
		} );

	crShaderBufferDescriptor Descriptor;
	Descriptor.AccessType = crShaderBufferAccessType_Static;
	Descriptor.BufferType = crShaderBufferType_Array;
	Descriptor.Capacity = sizeof ( Vertices );
	Descriptor.Data = &Vertices;
	Descriptor.DataSize = sizeof ( Vertices );
	crShaderBufferHandle VertexShaderBufferHandle = crCreateShaderBuffer ( Descriptor );
	if ( !VertexShaderBufferHandle )
		return -1;

	crShaderHandle ShaderHandle = crLoadShader ( DATA_PATH "/Shaders/OpenGLCore/TexturedQuadTest.vert",
	                              NULL,
	                              DATA_PATH "/Shaders/OpenGLCore/TexturedQuadTest.frag" );
	if ( !ShaderHandle )
		return -1;

	crTextureHandle TextureHandle = crLoadTexture ( DATA_PATH "/batman.jpg", true );
	if ( !TextureHandle )
		return -1;

	crFramebufferDescriptor FramebufferDescriptor = { 0 };
	FramebufferDescriptor.ColorAttachments = 1;
	FramebufferDescriptor.ColorAttachmentFormat = crPixelFormat_RedGreenBlueAlpha8888;
	FramebufferDescriptor.DepthEnabled = true;
	FramebufferDescriptor.DepthFormat = crPixelFormat_DepthComponent;
	FramebufferDescriptor.Dimensions = WindowSize;
	FramebufferDescriptor.ClearDepth = 1.0f;
	math_vec4_set ( &FramebufferDescriptor.ClearColor, 0.5f, 0.5f, 0.5f, 1.0f );

	crFramebufferHandle Framebuffer = crCreateFramebuffer ( FramebufferDescriptor );
	if ( !Framebuffer )
		return -1;

	crShaderBufferDataStream PositionStream;
	PositionStream.BufferHandle = VertexShaderBufferHandle;
	PositionStream.ComponentsPerElement = 3;
	PositionStream.ComponentType = crShaderBufferComponentType_Float;
	PositionStream.NormalizeData = false;
	PositionStream.PerInstance = false;
	PositionStream.StartOffset = offsetof ( struct Vertex, Position );
	PositionStream.Stride = sizeof ( struct Vertex );

	crShaderBufferDataStream TexCoordStream;
	TexCoordStream.BufferHandle = VertexShaderBufferHandle;
	TexCoordStream.ComponentsPerElement = 2;
	TexCoordStream.ComponentType = crShaderBufferComponentType_Float;
	TexCoordStream.NormalizeData = false;
	TexCoordStream.PerInstance = false;
	TexCoordStream.StartOffset = offsetof ( struct Vertex, TexCoord );
	TexCoordStream.Stride = sizeof ( struct Vertex );

	crTextureBindSettings TextureBindSettings = crDefaultTextureBindSettings;
	TextureBindSettings.Handle = TextureHandle;
	mat4 MVP;
	math_mat4_dump ( ModelMatrix );
	math_mat4_dump ( ViewMatrix );
	math_mat4_dump ( ProjectionMatrix );

	math_mat4_multiply ( &MVP, ViewMatrix, ModelMatrix );
	math_mat4_multiply ( &MVP, ProjectionMatrix, MVP );

	crRenderCommand RenderCommand = { 0 };
	crSetRenderCommandShader ( &RenderCommand, ShaderHandle );
	crSetRenderCommandShaderBufferBinding ( &RenderCommand, crGetShaderAttributeHandle ( ShaderHandle, "a_VertexPosition" ), PositionStream );
	crSetRenderCommandShaderBufferBinding ( &RenderCommand, crGetShaderAttributeHandle ( ShaderHandle, "a_TexCoord" ), TexCoordStream );
	crSetRenderCommandUniformMatrix4Value ( &RenderCommand, crGetShaderUniformHandle ( ShaderHandle, "u_MVP" ), MVP );
	crSetRenderCommandTextureBinding ( &RenderCommand, crGetShaderUniformHandle ( ShaderHandle, "u_Texture" ), TextureBindSettings );
	RenderCommand.InstanceCount = 1;
	RenderCommand.Primitive = crTriangleStrip;
	RenderCommand.StartVertex = 0;
	RenderCommand.VertexCount = 4;
	RenderCommand.InstanceCount = 1;
	RenderCommand.State.Culling.Enabled = true;

	while ( ShouldQuit == false )
		{
		crStartRenderToWindow ( crGetMainWindowHandle() );
		crClearFramebufferWithDefaultValues ( Framebuffer );

		crRunCommand ( RenderCommand );
		/*if (SpecificFrame((float)TimeDelta) == false)
			{
			ShouldQuit = true;
			break;
			}*/
		crDisplayFramebuffer ( Framebuffer, crGetMainWindowHandle() );
		crDisplayWindow ( crGetMainWindowHandle() );
		crUpdateWindows ( true );
		}
	return 0;
	}
