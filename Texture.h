#pragma once
#include <Math/mathlib.h>
#include <Platform/defines.h>
#include "PixelFormat.h"
#include <stdint.h>
#include <stdbool.h>

typedef void *crTextureHandle;
#define crTextureHandle_Invalid NULL

enum crTextureFilter
	{
	crTextureFilter_Linear = 0,
	crTextureFilter_Nearest,
	crTextureFilter_NearestMipmapNearest,
	crTextureFilter_NearestMipmapLinear,
	crTextureFilter_LinearMipmapLinear,
	crTextureFilter_LinearMipmapNearest
	};

enum crTextureWrapMode
	{
	crTextureWrapMode_Repeat = 0,
	crTextureWrapMode_MirroredRepeat,
	crTextureWrapMode_ClampToEdge,
	crTextureWrapMode_ClampToBorder
	};

enum crTextureType
	{
	crTextureType_Texture2D = 0,
	crTextureType_TextureCubeMap
	};

struct crTextureFilterSettings
	{
	enum crTextureFilter MinFilter, MagFilter;
	};

struct crTextureWrapSettings
	{
	enum crTextureWrapMode Horizontal, Vertical;
	};

struct crTextureDescriptor
	{
	uvec2 Dimensions;
	uint8_t *Data;
	enum crPixelFormat Format;
	enum crTextureType Type;
	struct crTextureFilterSettings FilterSettings;
	struct crTextureWrapSettings WrapSettings;
	bool Mipmapped;
	};

struct crTextureBindSettings
	{
	crTextureHandle Handle;
	struct crTextureWrapSettings WrapSettings;
	struct crTextureFilterSettings FilterSettings;
	};

inline void crDestroyTextureDescriptor ( struct crTextureDescriptor *Descriptor )
	{
	SAFE_DEL_C ( Descriptor->Data );
	}

extern const struct crTextureBindSettings crDefaultTextureBindSettings;
