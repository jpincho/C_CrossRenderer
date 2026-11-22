#include "PixelFormat.h"

struct BlockDescription
	{
	unsigned Width, Height, Bytes;
	};

static const struct BlockDescription BlockInfo[crPixelFormat_Count] =
	{
		[crPixelFormat_BC1_RGB] = { 4, 4, 8 },
		[crPixelFormat_BC1_RGB_SRGB] = { 4, 4, 8 },
		[crPixelFormat_BC1_RGBA] = { 4, 4, 8 },
		[crPixelFormat_BC1_RGBA_SRGB] = { 4, 4, 8 },
		[crPixelFormat_BC2] = { 4, 4, 16 },
		[crPixelFormat_BC2_SRGB] = { 4, 4, 16 },
		[crPixelFormat_BC3] = { 4, 4, 16 },
		[crPixelFormat_BC3_SRGB] = { 4, 4, 16 },
		[crPixelFormat_BC4] = { 4, 4, 8 },
		[crPixelFormat_BC4_Signed] = { 4, 4, 8 },
		[crPixelFormat_BC5] = { 4, 4, 16 },
		[crPixelFormat_BC5_Signed] = { 4, 4, 16 },
		[crPixelFormat_BC6H] = { 4, 4, 16 },
		[crPixelFormat_BC6H_Signed] = { 4, 4, 16 },
		[crPixelFormat_BC7] = { 4, 4, 16 },
		[crPixelFormat_BC7_SRGB] = { 4, 4, 16 },
		[crPixelFormat_ETC2_RGB] = { 4, 4, 8 },
		[crPixelFormat_ETC2_RGB_SRGB] = { 4, 4, 8 },
		[crPixelFormat_ETC2_RGBA1] = { 4, 4, 8 },
		[crPixelFormat_ETC2_RGBA1_SRGB] = { 4, 4, 8 },
		[crPixelFormat_ETC2_RGBA] = { 4, 4, 16 },
		[crPixelFormat_ETC2_RGBA_SRGB] = { 4, 4, 16 },
		[crPixelFormat_EAC_R11] = { 4, 4, 8 },
		[crPixelFormat_EAC_R11_Signed] = { 4, 4, 8 },
		[crPixelFormat_EAC_RG11] = { 4, 4, 16 },
		[crPixelFormat_EAC_RG11_Signed] = { 4, 4, 16 },
		[crPixelFormat_ASTC_4x4] = { 4, 4, 16 },
		[crPixelFormat_ASTC_4x4_SRGB] = { 4, 4, 16 },
		[crPixelFormat_ASTC_5x4] = { 5, 4, 16 },
		[crPixelFormat_ASTC_5x4_SRGB] = { 5, 4, 16 },
		[crPixelFormat_ASTC_5x5] = { 5, 5, 16 },
		[crPixelFormat_ASTC_5x5_SRGB] = { 5, 5, 16 },
		[crPixelFormat_ASTC_6x5] = { 6, 5, 16 },
		[crPixelFormat_ASTC_6x5_SRGB] = { 6, 5, 16 },
		[crPixelFormat_ASTC_6x6] = { 6, 6, 16 },
		[crPixelFormat_ASTC_6x6_SRGB] = { 6, 6, 16 },
		[crPixelFormat_ASTC_8x5] = { 8, 5, 16 },
		[crPixelFormat_ASTC_8x5_SRGB] = { 8, 5, 16 },
		[crPixelFormat_ASTC_8x6] = { 8, 6, 16 },
		[crPixelFormat_ASTC_8x6_SRGB] = { 8, 6, 16 },
		[crPixelFormat_ASTC_8x8] = { 8, 8, 16 },
		[crPixelFormat_ASTC_8x8_SRGB] = { 8, 8, 16 },
		[crPixelFormat_ASTC_10x5] = { 10, 5, 16 },
		[crPixelFormat_ASTC_10x5_SRGB] = { 10, 5, 16 },
		[crPixelFormat_ASTC_10x6] = { 10, 6, 16 },
		[crPixelFormat_ASTC_10x6_SRGB] = { 10, 6, 16 },
		[crPixelFormat_ASTC_10x8] = { 10, 8, 16 },
		[crPixelFormat_ASTC_10x8_SRGB] = { 10, 8, 16 },
		[crPixelFormat_ASTC_10x10] = { 10, 10, 16 },
		[crPixelFormat_ASTC_10x10_SRGB] = { 10, 10, 16 },
		[crPixelFormat_ASTC_12x10] = { 12, 10, 16 },
		[crPixelFormat_ASTC_12x10_SRGB] = { 12, 10, 16 },
		[crPixelFormat_ASTC_12x12] = { 12, 12, 16 },
		[crPixelFormat_ASTC_12x12_SRGB] = { 12, 12, 16 },
	};

int BytesPerPixel ( const crPixelFormat Format )
	{
	switch ( Format )
		{
		case crPixelFormat_Red8:
			return 1;
		case crPixelFormat_RedGreenBlue565:
		case crPixelFormat_RedGreen88:
			return 2;
		case crPixelFormat_RedGreenBlue888:
			return 3;
		case crPixelFormat_RedGreenBlueAlpha8888:
		case crPixelFormat_AlphaRedGreenBlue8888:
		case crPixelFormat_DepthComponent:
			return 4;
		default:
			return 0;
		}
	}

bool crPixelFormatIsCompressed ( const crPixelFormat Format )
	{
	return ( Format < crPixelFormat_Count ) && ( BlockInfo[Format].Bytes != 0 );
	}

bool crPixelFormatBlockSize ( const crPixelFormat Format, unsigned *BlockWidth, unsigned *BlockHeight, unsigned *BlockBytes )
	{
	if ( crPixelFormatIsCompressed ( Format ) == false )
		return false;
	*BlockWidth = BlockInfo[Format].Width;
	*BlockHeight = BlockInfo[Format].Height;
	*BlockBytes = BlockInfo[Format].Bytes;
	return true;
	}

unsigned crPixelFormatDataSize ( const crPixelFormat Format, const unsigned Width, const unsigned Height )
	{
	if ( Format >= crPixelFormat_Count )
		return 0;
	if ( BlockInfo[Format].Bytes == 0 )
		return Width * Height * ( unsigned ) BytesPerPixel ( Format );

	const unsigned BlocksX = ( Width + BlockInfo[Format].Width - 1 ) / BlockInfo[Format].Width;
	const unsigned BlocksY = ( Height + BlockInfo[Format].Height - 1 ) / BlockInfo[Format].Height;
	return BlocksX * BlocksY * BlockInfo[Format].Bytes;
	}
