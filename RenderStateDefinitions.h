#pragma once
#include <Math/mathlib.h>
#include <stdbool.h>
#include "Framebuffer.h"

typedef enum
    {
    crBlendMode_Zero = 0,
    crBlendMode_One,
    crBlendMode_SourceColor,
    crBlendMode_OneMinusSourceColor,
    crBlendMode_SourceAlpha,
    crBlendMode_OneMinusSourceAlpha,
    crBlendMode_DestinationAlpha,
    crBlendMode_OneMinusDestinationAlpha,
    crBlendMode_DestinationColor,
    crBlendMode_OneMinusDestinationColor,
    crBlendMode_SourceAlphaSaturate,
    } crBlendMode;

typedef enum
    {
    crStencilFunction_Never = 0,
    crStencilFunction_Less,
    crStencilFunction_LessOrEqual,
    crStencilFunction_Greater,
    crStencilFunction_GreaterOrEqual,
    crStencilFunction_Equal,
    crStencilFunction_NotEqual,
    crStencilFunction_Always
    } crStencilFunction;

typedef enum
    {
    crStencilFailAction_Zero = 0,
    crStencilFailAction_Keep,
    crStencilFailAction_Replace,
    crStencilFailAction_Increment,
    crStencilFailAction_Decrement,
    crStencilFailAction_Invert
    } crStencilFailAction;

typedef enum
    {
    crCullingMode_Front = 0,
    crCullingMode_Back,
    crCullingMode_FrontAndBack
    } crCullingMode;

typedef enum
    {
    crCullingFaceWinding_Clockwise = 0,
    crCullingFaceWinding_CounterClockwise
    } crCullingFaceWinding;

typedef enum
    {
    crDepthTestMode_Never = 0,
    crDepthTestMode_Less,
    crDepthTestMode_LessOrEqual,
    crDepthTestMode_Greater,
    crDepthTestMode_GreaterOrEqual,
    crDepthTestMode_Equal,
    crDepthTestMode_NotEqual,
    crDepthTestMode_Always
    } crDepthTestMode;

typedef enum
    {
    crPolygonMode_Point = 0,
    crPolygonMode_Line,
    crPolygonMode_Fill
    } crPolygonMode;

typedef struct
    {
    bool Enabled;
    crBlendMode Source, Destination;
    } crBlendSettings;

typedef struct
    {
    bool Enabled;
    unsigned Mask;
    crStencilFunction Function;
    unsigned FunctionReference, FunctionMask;
    crStencilFailAction OnFail, OnFailZ, OnPassZ;
    } crStencilBufferSettings;

typedef struct
    {
    bool Enabled;
    uvec2 LowerLeft, Dimensions;
    } crScissorSettings;

typedef struct
    {
    bool Enabled;
    uvec2 LowerLeft, Dimensions;
    } crViewportSettings;

typedef struct
    {
    bool Enabled;
    crCullingMode Mode;
    crCullingFaceWinding Winding;
    } crCullingSettings;

typedef struct
    {
    bool Enabled;
    crDepthTestMode Mode;
    } crDepthTestSettings;

typedef struct
    {
    crFramebufferHandle TargetFramebuffer;
    } crFramebufferSettings;

typedef struct
    {
    crPolygonMode State;
    } crPolygonModeSettings;

typedef struct
    {
    crBlendSettings Blending;
    crCullingSettings Culling;
    crStencilBufferSettings Stencil;
    crScissorSettings Scissor;
    crViewportSettings Viewport;
    crDepthTestSettings DepthTest;
    crPolygonModeSettings PolygonMode;
    } crRenderState;
