#pragma once
#include <Math/mathlib.h>
#include <Platform/defines.h>
#include <stdint.h>
#include <stdbool.h>
#include "PixelFormat.h"

typedef void *crFramebufferHandle;
#define FramebufferHandle_Invalid NULL

typedef struct
	{
	vec4 ClearColor;
	bool DepthEnabled;
	uint8_t ColorAttachments;
	crPixelFormat ColorAttachmentFormat, DepthFormat;
	uvec2 Dimensions;
	float ClearDepth;
	} crFramebufferDescriptor;
