#pragma once
#include <Math/mathlib.h>
#include <Platform/defines.h>
#include <stdint.h>
#include <stdbool.h>

typedef void *crFramebufferHandle;
#define FramebufferHandle_Invalid NULL

struct crFramebufferDescriptor
	{
	vec4 ClearColor;
	bool DepthEnabled;
	uint8_t ColorAttachments;
	enum crPixelFormat ColorAttachmentFormat, DepthFormat;
	uvec2 Dimensions;
	float ClearDepth;
	};
