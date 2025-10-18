#include <CrossRenderer.h>
#include <Platform/Logger.h>
#include "../CommonLoaders.h"

#define _USE_MATH_DEFINES
#include <math.h>

static bool ShouldQuit = false;
bool LoadFileContents(const char *Filename, char **Contents);

void WindowClosed_Callback(const crWindowHandle Handle)
    {
    ShouldQuit = true;
    }

void WindowMoved_Callback(const crWindowHandle Handle, const ivec2 NewPosition)
    {
    LOG_DEBUG("Window moved to %d %d", NewPosition.x, NewPosition.y);
    }

void WindowResized_Callback(const crWindowHandle Handle, const ivec2 NewSize)
    {
    LOG_DEBUG("Window resized to %d %d", NewSize.x, NewSize.y);
    }

void MouseClicked_Callback(const crWindowHandle Handle, const int Button, const bool Click)
    {
    LOG_DEBUG("Mouse button %u %s", Button, Click ? "clicked" : "released");
    }

void MouseWheel_Callback(const crWindowHandle Handle, const int Delta)
    {
    LOG_DEBUG("Mouse wheel %d", Delta);
    }

void MouseMoved_Callback(const crWindowHandle Handle, const ivec2 Delta, const ivec2 NewPosition)
    {
    LOG_DEBUG("Mouse moved %d %d %d %d", Delta.x, Delta.y, NewPosition.x, NewPosition.y);
    }

void WindowFocusChanged_Callback(const crWindowHandle Handle, const bool HasFocus)
    {
    LOG_DEBUG("Window focus %s", HasFocus ? "true" : "false");
    }

int main(int argc, char *argv[])
    {
    crRendererConfiguration Configuration = { 0 };
    Configuration.InitialWindowDescriptor.Fullscreen = false;
    Configuration.InitialWindowDescriptor.Position.x = 100;
    Configuration.InitialWindowDescriptor.Position.y = 100;
    Configuration.InitialWindowDescriptor.Size.x = 1920;
    Configuration.InitialWindowDescriptor.Size.y = 1080;
    Configuration.InitialWindowDescriptor.Resizable = true;
    Configuration.InitialWindowDescriptor.Title = "CrossRenderer - textured quad test";
    Configuration.VSyncEnabled = true;
    if (crInitialize(Configuration) == false)
        return -1;

    crWindowManagerCallbacks Callbacks;
    memset(&Callbacks, 0, sizeof(Callbacks));
    Callbacks.EndWindowResized = WindowResized_Callback;
    Callbacks.WindowClosed = WindowClosed_Callback;
    Callbacks.WindowMoved = WindowMoved_Callback;
    Callbacks.MouseButton = MouseClicked_Callback;
    Callbacks.MouseWheel = MouseWheel_Callback;
    Callbacks.MouseMoved = MouseMoved_Callback;
    Callbacks.WindowFocusChanged = WindowFocusChanged_Callback;
    crSetWindowManagerCallbacks(Callbacks);

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
    crGetWindowDimensions(crGetMainWindowHandle(), &WindowSize);
    mat4 ProjectionMatrix;
    math_mat4_set_perspective_matrix(&ProjectionMatrix, M_PI_2, (float)WindowSize.x / (float)WindowSize.y, 0.1f, 10.0f);
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

    crShaderBufferDescriptor Descriptor;
    Descriptor.AccessType = crShaderBufferAccessType_Static;
    Descriptor.BufferType = crShaderBufferType_Array;
    Descriptor.Capacity = sizeof(Vertices);
    Descriptor.Data = &Vertices;
    Descriptor.DataSize = sizeof(Vertices);
    crShaderBufferHandle VertexShaderBufferHandle = crCreateShaderBuffer(Descriptor);
    if (!VertexShaderBufferHandle)
        return -1;

    crShaderHandle ShaderHandle = LoadShader(DATA_PATH "/Shaders/OpenGLCore/TexturedQuadTest.vert",
                                  NULL,
                                  DATA_PATH "/Shaders/OpenGLCore/TexturedQuadTest.frag");
    if (!ShaderHandle)
        return -1;

    crTextureHandle TextureHandle = LoadTexture(DATA_PATH "/batman.jpg", true);
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

    crTextureBindSettings TextureBindSettings = crDefaultTextureBindSettings;
    TextureBindSettings.Handle = TextureHandle;
    mat4 MVP, MV;
    math_mat4_dump(ModelMatrix);
    math_mat4_dump(ViewMatrix);
    math_mat4_dump(ProjectionMatrix);

    math_mat4_multiply_to(&MVP, ViewMatrix, ModelMatrix);
    math_mat4_multiply_to(&MVP, ProjectionMatrix, MVP);

    //math_mat4_multiply_multiple_to(&MVP, 3, ProjectionMatrix, ViewMatrix, ModelMatrix);
    //math_mat4_set_identity_matrix(&MVP);
    //math_mat4_dump(MVP);
    //math_mat4_multiply_to(&MV, ProjectionMatrix, ViewMatrix);
    //math_mat4_multiply_to(&MVP, MV, ModelMatrix);
    //math_mat4_multiply_inplace(&MVP, &ModelMatrix);
    //math_mat4_multiply_inplace(&MVP, &ViewMatrix);
    //math_mat4_multiply_inplace(&MVP, &ProjectionMatrix);
    crRenderCommand RenderCommand = { 0 };
    crSetRenderCommandShader(&RenderCommand, ShaderHandle);
    crSetRenderCommandShaderBufferBinding(&RenderCommand, crGetShaderAttributeHandle(ShaderHandle, "a_VertexPosition"), PositionStream);
    crSetRenderCommandShaderBufferBinding(&RenderCommand, crGetShaderAttributeHandle(ShaderHandle, "a_TexCoord"), TexCoordStream);
    crSetRenderCommandUniformMatrix4Value(&RenderCommand, crGetShaderUniformHandle(ShaderHandle, "u_MVP"), MVP);
    crSetRenderCommandTextureBinding(&RenderCommand, crGetShaderUniformHandle(ShaderHandle, "u_Texture"), TextureBindSettings);
    RenderCommand.InstanceCount = 1;
    RenderCommand.Primitive = crTriangleStrip;
    RenderCommand.StartVertex = 0;
    RenderCommand.VertexCount = 4;
    RenderCommand.InstanceCount = 1;
    RenderCommand.State.Culling.Enabled = true;

    while (ShouldQuit == false)
        {
        crStartRenderToWindow(crGetMainWindowHandle());
        crClearFramebufferWithDefaultValues(Framebuffer);

        crRunCommand(RenderCommand);
        /*if (SpecificFrame((float)TimeDelta) == false)
        	{
        	ShouldQuit = true;
        	break;
        	}*/
        crDisplayFramebuffer(Framebuffer, crGetMainWindowHandle());
        crDisplayWindow(crGetMainWindowHandle());
        crUpdateWindows(true);
        }
    return 0;
    }