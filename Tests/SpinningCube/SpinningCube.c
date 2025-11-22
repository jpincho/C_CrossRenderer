#include <CrossRenderer.h>
#include <Platform/Logger.h>
#include <Platform/PlatformTime.h>
#include "../CommonLoaders.h"

#define _USE_MATH_DEFINES
#include <math.h>

static bool ShouldQuit = false;
bool LoadFileContents(const char *Filename, char **Contents);

void WindowClosed_Callback(const crWindowHandle Handle)
	{
	UNUSED(Handle);
	ShouldQuit = true;
	}

int main(void)
	{
	crRendererConfiguration Configuration = { 0 };
	Configuration.InitialWindowDescriptor.Fullscreen = false;
	Configuration.InitialWindowDescriptor.Position.x = 100;
	Configuration.InitialWindowDescriptor.Position.y = 100;
	Configuration.InitialWindowDescriptor.Size.x = 1920;
	Configuration.InitialWindowDescriptor.Size.y = 1080;
	Configuration.InitialWindowDescriptor.Resizable = true;
	Configuration.InitialWindowDescriptor.Title = "CrossRenderer - spinning cube test";
	Configuration.VSyncEnabled = true;
	if (crInitialize(Configuration) == false)
		return -1;

	crWindowManagerCallbacks Callbacks = { 0 };
	Callbacks.WindowClosed = WindowClosed_Callback;
	crSetWindowManagerCallbacks(Callbacks);

	struct Vertex
		{
		vec3 Position;
		vec2 TexCoord;
		};

	struct Vertex Vertices[24] =
		{
			{{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f}},
			{{ 1.0f, -1.0f, 1.0f}, {1.0f, 0.0f}},
			{{-1.0f,  1.0f, 1.0f}, {0.0f, 1.0f}},
			{{ 1.0f,  1.0f, 1.0f}, {1.0f, 1.0f}},

			{{ 1.0f, -1.0f, 1.0f}, {0.0f, 0.0f}},
			{{ 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}},
			{{ 1.0f,  1.0f, 1.0f}, {0.0f, 1.0f}},
			{{ 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f}},

			{{ 1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}},
			{{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}},
			{{ 1.0f,  1.0f, -1.0f}, {0.0f, 1.0f}},
			{{-1.0f,  1.0f, -1.0f}, {1.0f, 1.0f}},

			{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}},
			{{-1.0f, -1.0f, 1.0f}, {1.0f, 0.0f}},
			{{-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f}},
			{{-1.0f,  1.0f, 1.0f}, {1.0f, 1.0f}},

			{{-1.0f,  1.0f, 1.0f}, {0.0f, 0.0f}},
			{{ 1.0f,  1.0f, 1.0f}, {1.0f, 0.0f}},
			{{-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f}},
			{{ 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f}},

			{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}},
			{{ 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}},
			{{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f}},
			{{ 1.0f, -1.0f, 1.0f}, {1.0f, 1.0f}},
		};
	uint16_t Indices[] =
		{
		0, 1, 2,
		1, 3, 2,

		0 + 4, 1 + 4, 2 + 4,
		1 + 4, 3 + 4, 2 + 4,

		0 + 8, 1 + 8, 2 + 8,
		1 + 8, 3 + 8, 2 + 8,

		0 + 12, 1 + 12, 2 + 12,
		1 + 12, 3 + 12, 2 + 12,

		0 + 16, 1 + 16, 2 + 16,
		1 + 16, 3 + 16, 2 + 16,

		0 + 20, 1 + 20, 2 + 20,
		1 + 20, 3 + 20, 2 + 20,
		};

	uvec2 WindowSize;
	crGetWindowDimensions(crGetMainWindowHandle(), &WindowSize);
	mat4 ProjectionMatrix;
	math_mat4_set_perspective_matrix(&ProjectionMatrix, (float)M_PI_2, (float)WindowSize.x / (float)WindowSize.y, 0.1f, 10.0f);
	mat4 ModelMatrix;
	math_mat4_set_identity_matrix(&ModelMatrix);
	mat4 ViewMatrix;
	math_mat4_set_view_matrix(&ViewMatrix, (vec3)
		{
		0.0f, 0.0f, 2.0f
		}, (vec3)
		{
		0.0f, 0.0f, 0.0f
		}, (vec3)
		{
		0, 1.0f, 0
		});

	crShaderBufferDescriptor VertexSBDescriptor;
	VertexSBDescriptor.AccessType = crShaderBufferAccessType_Static;
	VertexSBDescriptor.BufferType = crShaderBufferType_Array;
	VertexSBDescriptor.Capacity = sizeof(Vertices);
	VertexSBDescriptor.Data = &Vertices;
	VertexSBDescriptor.DataSize = sizeof(Vertices);
	crShaderBufferHandle VertexShaderBufferHandle = crCreateShaderBuffer(VertexSBDescriptor);
	if (!VertexShaderBufferHandle)
		return -1;

	crShaderBufferDescriptor IndexSBDescriptor;
	IndexSBDescriptor.AccessType = crShaderBufferAccessType_Static;
	IndexSBDescriptor.BufferType = crShaderBufferType_Array;
	IndexSBDescriptor.Capacity = sizeof(Indices);
	IndexSBDescriptor.Data = &Indices;
	IndexSBDescriptor.DataSize = sizeof(Indices);
	crShaderBufferHandle IndexShaderBufferHandle = crCreateShaderBuffer(IndexSBDescriptor);
	if (!IndexShaderBufferHandle)
		return -1;

	crShaderHandle ShaderHandle = LoadShader(DATA_PATH "/Shaders/OpenGLCore/SpinningCubeTest.vert",
	                              NULL,
	                              DATA_PATH "/Shaders/OpenGLCore/SpinningCubeTest.frag");
	if (!ShaderHandle)
		return -1;

	crTextureHandle TextureHandle = LoadTexture(DATA_PATH "../TexturedQuad/batman.jpg", true);
	if (!TextureHandle)
		return -1;

	crFramebufferDescriptor FramebufferDescriptor = { 0 };
	FramebufferDescriptor.ColorAttachments = 1;
	FramebufferDescriptor.ColorAttachmentFormat = crPixelFormat_RedGreenBlueAlpha8888;
	FramebufferDescriptor.DepthEnabled = true;
	FramebufferDescriptor.DepthFormat = crPixelFormat_DepthComponent;
	FramebufferDescriptor.Dimensions = WindowSize;
	FramebufferDescriptor.ClearDepth = 1.0f;
	math_vec4_set(&FramebufferDescriptor.ClearColor, 0.5f, 0.5f, 0.5f, 1.0f);

	crFramebufferHandle Framebuffer = crCreateFramebuffer(FramebufferDescriptor);
	if (!Framebuffer)
		return -1;

	crShaderBufferDataStream PositionStream;
	PositionStream.BufferHandle = VertexShaderBufferHandle;
	PositionStream.ComponentsPerElement = 3;
	PositionStream.ComponentType = crShaderBufferComponentType_Float;
	PositionStream.NormalizeData = false;
	PositionStream.PerInstance = false;
	PositionStream.StartOffset = offsetof(struct Vertex, Position);
	PositionStream.Stride = sizeof(struct Vertex);

	crShaderBufferDataStream TexCoordStream;
	TexCoordStream.BufferHandle = VertexShaderBufferHandle;
	TexCoordStream.ComponentsPerElement = 2;
	TexCoordStream.ComponentType = crShaderBufferComponentType_Float;
	TexCoordStream.NormalizeData = false;
	TexCoordStream.PerInstance = false;
	TexCoordStream.StartOffset = offsetof(struct Vertex, TexCoord);
	TexCoordStream.Stride = sizeof(struct Vertex);

	crShaderBufferDataStream IndexStream;
	IndexStream.BufferHandle = IndexShaderBufferHandle;
	IndexStream.ComponentsPerElement = 1;
	IndexStream.ComponentType = crShaderBufferComponentType_Unsigned16;
	IndexStream.NormalizeData = false;
	IndexStream.PerInstance = false;
	IndexStream.StartOffset = 0;
	IndexStream.Stride = sizeof(uint16_t);

	crTextureBindSettings TextureBindSettings = crDefaultTextureBindSettings;
	TextureBindSettings.Handle = TextureHandle;
	mat4 MVP;
	math_mat4_dump(ModelMatrix);
	math_mat4_dump(ViewMatrix);
	math_mat4_dump(ProjectionMatrix);

	math_mat4_multiply_to(&MVP, ViewMatrix, ModelMatrix);
	math_mat4_multiply_to(&MVP, ProjectionMatrix, MVP);

	crRenderCommand RenderCommand = { 0 };
	crSetRenderCommandShader(&RenderCommand, ShaderHandle);
	crSetRenderCommandShaderBufferBinding(&RenderCommand, crGetShaderAttributeHandle(ShaderHandle, "a_VertexPosition"), PositionStream);
	crSetRenderCommandShaderBufferBinding(&RenderCommand, crGetShaderAttributeHandle(ShaderHandle, "a_TexCoord"), TexCoordStream);
	crSetRenderCommandIndexShaderBufferBinding(&RenderCommand, IndexStream);
	crSetRenderCommandUniformMatrix4Value(&RenderCommand, crGetShaderUniformHandle(ShaderHandle, "u_MVP"), MVP);
	crSetRenderCommandTextureBinding(&RenderCommand, crGetShaderUniformHandle(ShaderHandle, "u_Texture"), TextureBindSettings);
	RenderCommand.InstanceCount = 1;
	RenderCommand.Primitive = crTriangleList;
	RenderCommand.StartVertex = 0;
	RenderCommand.VertexCount = 36;
	RenderCommand.InstanceCount = 1;
	RenderCommand.State.Culling.Enabled = true;
	RenderCommand.State.Culling.Mode = crCullingMode_Back;
	RenderCommand.State.Culling.Winding = crCullingFaceWinding_CounterClockwise;
	RenderCommand.State.DepthTest.Enabled = true;
	RenderCommand.State.DepthTest.Mode = crDepthTestMode_Less;

	double LastEndFrameSeconds = Platform_GetPerformanceCurrentTime();
	double TotalDelta = 0.0;
	while (ShouldQuit == false)
		{
		double Seconds = Platform_GetPerformanceCurrentTime();
		double TimeDelta = Seconds - LastEndFrameSeconds;
		if (TimeDelta == 0.0)
			TimeDelta = 0.01;
		LastEndFrameSeconds = Seconds;
		TotalDelta += TimeDelta;
		crStartRenderToWindow(crGetMainWindowHandle());
		crClearFramebufferWithDefaultValues(Framebuffer);

		math_mat4_set_rotation_matrix(&ModelMatrix, (float)(TotalDelta * M_PI_4), (vec3)
			{
			0, 1, 0
			});
		math_mat4_multiply_to(&MVP, ViewMatrix, ModelMatrix);
		math_mat4_multiply_to(&MVP, ProjectionMatrix, MVP);
		crSetRenderCommandUniformMatrix4Value(&RenderCommand, crGetShaderUniformHandle(ShaderHandle, "u_MVP"), MVP);

		crRunCommand(RenderCommand);
		crDisplayFramebuffer(Framebuffer, crGetMainWindowHandle());
		crDisplayWindow(crGetMainWindowHandle());
		crUpdateWindows(false);
		}
	return 0;
	}