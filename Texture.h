#pragma once
#include <Math/mathlib.h>
#include <Platform/defines.h>
#include "PixelFormat.h"
#include <stdint.h>
#include <stdbool.h>

BEGIN_C_DECLARATIONS
typedef void *crTextureHandle;
#define crTextureHandle_Invalid NULL

typedef enum
	{
	crTextureFilter_Linear = 0,
	crTextureFilter_Nearest,
	crTextureFilter_NearestMipmapNearest,
	crTextureFilter_NearestMipmapLinear,
	crTextureFilter_LinearMipmapLinear,
	crTextureFilter_LinearMipmapNearest
	} crTextureFilter;

typedef enum
	{
	crTextureWrapMode_Repeat = 0,
	crTextureWrapMode_MirroredRepeat,
	crTextureWrapMode_ClampToEdge,
	crTextureWrapMode_ClampToBorder
	} crTextureWrapMode;

typedef enum
	{
	crTextureType_Texture2D = 0,
	crTextureType_TextureCubeMap
	} crTextureType;

typedef struct
	{
	crTextureFilter MinFilter, MagFilter;
	} crTextureFilterSettings;

typedef struct
	{
	crTextureWrapMode Horizontal, Vertical;
	} crTextureWrapSettings;

typedef struct
	{
	uvec2 Dimensions;
	uint8_t *Data;
	crPixelFormat Format;
	crTextureType Type;
	crTextureFilterSettings FilterSettings;
	crTextureWrapSettings WrapSettings;
	bool Mipmapped;
	} crTextureDescriptor;

typedef struct
	{
	crTextureHandle Handle;
	crTextureWrapSettings WrapSettings;
	crTextureFilterSettings FilterSettings;
	} crTextureBindSettings;

inline void crDestroyTextureDescriptor ( crTextureDescriptor *Descriptor )
	{
	SAFE_DEL_C ( Descriptor->Data );
	}

extern const crTextureBindSettings crDefaultTextureBindSettings;
END_C_DECLARATIONS
