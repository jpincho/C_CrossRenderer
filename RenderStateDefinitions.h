#pragma once
#include <Math/mathlib.h>
#include <stdbool.h>
#include "Framebuffer.h"

enum crBlendMode
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
	};

enum crStencilFunction
	{
	crStencilFunction_Never = 0,
	crStencilFunction_Less,
	crStencilFunction_LessOrEqual,
	crStencilFunction_Greater,
	crStencilFunction_GreaterOrEqual,
	crStencilFunction_Equal,
	crStencilFunction_NotEqual,
	crStencilFunction_Always
	};

enum crStencilFailAction
	{
	crStencilFailAction_Zero = 0,
	crStencilFailAction_Keep,
	crStencilFailAction_Replace,
	crStencilFailAction_Increment,
	crStencilFailAction_Decrement,
	crStencilFailAction_Invert
	};

enum crCullingMode
	{
	crCullingMode_Front = 0,
	crCullingMode_Back,
	crCullingMode_FrontAndBack
	};

enum crCullingFaceWinding
	{
	crCullingFaceWinding_Clockwise = 0,
	crCullingFaceWinding_CounterClockwise
	};

enum crDepthTestMode
	{
	crDepthTestMode_Never = 0,
	crDepthTestMode_Less,
	crDepthTestMode_LessOrEqual,
	crDepthTestMode_Greater,
	crDepthTestMode_GreaterOrEqual,
	crDepthTestMode_Equal,
	crDepthTestMode_NotEqual,
	crDepthTestMode_Always
	};

enum crPolygonMode
	{
	crPolygonMode_Point = 0,
	crPolygonMode_Line,
	crPolygonMode_Fill
	};

struct crBlendSettings
	{
	bool Enabled;
	enum crBlendMode Source, Destination;
	};

struct crStencilBufferSettings
	{
	bool Enabled;
	unsigned Mask;
	enum crStencilFunction Function;
	unsigned FunctionReference, FunctionMask;
	enum crStencilFailAction OnFail, OnFailZ, OnPassZ;
	};

struct crScissorSettings
	{
	bool Enabled;
	uvec2 LowerLeft, Dimensions;
	};

struct crViewportSettings
	{
	bool Enabled;
	uvec2 LowerLeft, Dimensions;
	};

struct crCullingSettings
	{
	bool Enabled;
	enum crCullingMode Mode;
	enum crCullingFaceWinding Winding;
	};

struct crDepthTestSettings
	{
	bool Enabled;
	enum crDepthTestMode Mode;
	};

struct crFramebufferSettings
	{
	crFramebufferHandle TargetFramebuffer;
	};

struct crPolygonModeSettings
	{
	enum crPolygonMode State;
	};

struct crRenderState
	{
	struct crBlendSettings Blending;
	struct crCullingSettings Culling;
	struct crStencilBufferSettings Stencil;
	struct crScissorSettings Scissor;
	struct crViewportSettings Viewport;
	struct crDepthTestSettings DepthTest;
	struct crPolygonModeSettings PolygonMode;
	};
