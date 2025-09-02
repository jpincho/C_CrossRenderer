#pragma once
#include <stdbool.h>
#include <cglm/cglm.h>
#include <Platform/defines.h>

BEGIN_C_DECLARATIONS

typedef enum
	{
	PixelFormat_Red8 = 0,
	PixelFormat_RedGreen88,
	PixelFormat_RedGreenBlue888,
	PixelFormat_RedGreenBlueAlpha8888,
	PixelFormat_AlphaRedGreenBlue8888,
	PixelFormat_RedGreenBlue565,
	PixelFormat_DepthComponent,
	}EPixelFormat;

typedef enum
	{
	TextureFilter_Linear = 0,
	TextureFilter_Nearest,
	TextureFilter_NearestMipmapNearest,
	TextureFilter_NearestMipmapLinear,
	TextureFilter_LinearMipmapLinear,
	TextureFilter_LinearMipmapNearest
	}ETextureFilter;

typedef enum
	{
	TextureWrapMode_Repeat = 0,
	TextureWrapMode_MirroredRepeat,
	TextureWrapMode_ClampToEdge,
	TextureWrapMode_ClampToBorder
	}ETextureWrapMode;

typedef enum
	{
	TextureType_Texture2D = 0,
	TextureType_TextureCubeMap
	}ETextureType;

typedef struct
	{
	ETextureFilter min_filter, mag_filter;
	}texture_filter_settings_t;

typedef struct
	{
	ETextureWrapMode horizontal, vertical;
	}texture_wrap_settings_t;

typedef struct
	{
	char *name;
	ivec2 Dimensions;
	uint8_t *Data;
	EPixelFormat Format;
	ETextureType Type;
	texture_filter_settings_t FilterSettings;
	texture_wrap_settings_t WrapSettings;
	bool mipmapped;
	}texture_descriptor_t;

void destroy_texture_descriptor ( texture_descriptor_t *texture );
END_C_DECLARATIONS
