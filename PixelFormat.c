#include "PixelFormat.h"

static const struct crPixelFormatInfo FormatInfo[crPixelFormat_Count] =
	{
	[crPixelFormat_Red8] = { .Bytes = 1, .Mipmaps = true },
	[crPixelFormat_RedGreen88] = { .Bytes = 2, .Mipmaps = true },
	[crPixelFormat_RedGreenBlue888] = { .Bytes = 3, .Mipmaps = true },
	[crPixelFormat_RedGreenBlueAlpha8888] = { .Bytes = 4, .Mipmaps = true },
	[crPixelFormat_AlphaRedGreenBlue8888] = { .Bytes = 4, .Mipmaps = true },
	[crPixelFormat_RedGreenBlue565] = { .Bytes = 2, .Mipmaps = true },
	[crPixelFormat_DepthComponent] = { .Bytes = 4 },
	[crPixelFormat_BC1_RGB] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 8 },
	[crPixelFormat_BC1_RGB_SRGB] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 8 },
	[crPixelFormat_BC1_RGBA] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 8 },
	[crPixelFormat_BC1_RGBA_SRGB] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 8 },
	[crPixelFormat_BC2] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 16 },
	[crPixelFormat_BC2_SRGB] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 16 },
	[crPixelFormat_BC3] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 16 },
	[crPixelFormat_BC3_SRGB] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 16 },
	[crPixelFormat_BC4] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 8 },
	[crPixelFormat_BC4_Signed] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 8 },
	[crPixelFormat_BC5] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 16 },
	[crPixelFormat_BC5_Signed] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 16 },
	[crPixelFormat_BC6H] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 16 },
	[crPixelFormat_BC6H_Signed] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 16 },
	[crPixelFormat_BC7] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 16 },
	[crPixelFormat_BC7_SRGB] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 16 },
	[crPixelFormat_ETC2_RGB] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 8 },
	[crPixelFormat_ETC2_RGB_SRGB] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 8 },
	[crPixelFormat_ETC2_RGBA1] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 8 },
	[crPixelFormat_ETC2_RGBA1_SRGB] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 8 },
	[crPixelFormat_ETC2_RGBA] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 16 },
	[crPixelFormat_ETC2_RGBA_SRGB] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 16 },
	[crPixelFormat_EAC_R11] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 8 },
	[crPixelFormat_EAC_R11_Signed] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 8 },
	[crPixelFormat_EAC_RG11] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 16 },
	[crPixelFormat_EAC_RG11_Signed] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 16 },
	[crPixelFormat_ASTC_4x4] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 16 },
	[crPixelFormat_ASTC_4x4_SRGB] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 16 },
	[crPixelFormat_ASTC_5x4] = { .BlockWidth = 5, .BlockHeight = 4, .BlockBytes = 16 },
	[crPixelFormat_ASTC_5x4_SRGB] = { .BlockWidth = 5, .BlockHeight = 4, .BlockBytes = 16 },
	[crPixelFormat_ASTC_5x5] = { .BlockWidth = 5, .BlockHeight = 5, .BlockBytes = 16 },
	[crPixelFormat_ASTC_5x5_SRGB] = { .BlockWidth = 5, .BlockHeight = 5, .BlockBytes = 16 },
	[crPixelFormat_ASTC_6x5] = { .BlockWidth = 6, .BlockHeight = 5, .BlockBytes = 16 },
	[crPixelFormat_ASTC_6x5_SRGB] = { .BlockWidth = 6, .BlockHeight = 5, .BlockBytes = 16 },
	[crPixelFormat_ASTC_6x6] = { .BlockWidth = 6, .BlockHeight = 6, .BlockBytes = 16 },
	[crPixelFormat_ASTC_6x6_SRGB] = { .BlockWidth = 6, .BlockHeight = 6, .BlockBytes = 16 },
	[crPixelFormat_ASTC_8x5] = { .BlockWidth = 8, .BlockHeight = 5, .BlockBytes = 16 },
	[crPixelFormat_ASTC_8x5_SRGB] = { .BlockWidth = 8, .BlockHeight = 5, .BlockBytes = 16 },
	[crPixelFormat_ASTC_8x6] = { .BlockWidth = 8, .BlockHeight = 6, .BlockBytes = 16 },
	[crPixelFormat_ASTC_8x6_SRGB] = { .BlockWidth = 8, .BlockHeight = 6, .BlockBytes = 16 },
	[crPixelFormat_ASTC_8x8] = { .BlockWidth = 8, .BlockHeight = 8, .BlockBytes = 16 },
	[crPixelFormat_ASTC_8x8_SRGB] = { .BlockWidth = 8, .BlockHeight = 8, .BlockBytes = 16 },
	[crPixelFormat_ASTC_10x5] = { .BlockWidth = 10, .BlockHeight = 5, .BlockBytes = 16 },
	[crPixelFormat_ASTC_10x5_SRGB] = { .BlockWidth = 10, .BlockHeight = 5, .BlockBytes = 16 },
	[crPixelFormat_ASTC_10x6] = { .BlockWidth = 10, .BlockHeight = 6, .BlockBytes = 16 },
	[crPixelFormat_ASTC_10x6_SRGB] = { .BlockWidth = 10, .BlockHeight = 6, .BlockBytes = 16 },
	[crPixelFormat_ASTC_10x8] = { .BlockWidth = 10, .BlockHeight = 8, .BlockBytes = 16 },
	[crPixelFormat_ASTC_10x8_SRGB] = { .BlockWidth = 10, .BlockHeight = 8, .BlockBytes = 16 },
	[crPixelFormat_ASTC_10x10] = { .BlockWidth = 10, .BlockHeight = 10, .BlockBytes = 16 },
	[crPixelFormat_ASTC_10x10_SRGB] = { .BlockWidth = 10, .BlockHeight = 10, .BlockBytes = 16 },
	[crPixelFormat_ASTC_12x10] = { .BlockWidth = 12, .BlockHeight = 10, .BlockBytes = 16 },
	[crPixelFormat_ASTC_12x10_SRGB] = { .BlockWidth = 12, .BlockHeight = 10, .BlockBytes = 16 },
	[crPixelFormat_ASTC_12x12] = { .BlockWidth = 12, .BlockHeight = 12, .BlockBytes = 16 },
	[crPixelFormat_ASTC_12x12_SRGB] = { .BlockWidth = 12, .BlockHeight = 12, .BlockBytes = 16 },
	[crPixelFormat_Red8_SRGB] = { .Bytes = 1, .Mipmaps = true },
	[crPixelFormat_RedGreen88_SRGB] = { .Bytes = 2, .Mipmaps = true },
	[crPixelFormat_RedGreenBlue888_SRGB] = { .Bytes = 3, .Mipmaps = true },
	[crPixelFormat_RedGreenBlueAlpha8888_SRGB] = { .Bytes = 4, .Mipmaps = true },
	[crPixelFormat_Red8_Signed] = { .Bytes = 1, .Mipmaps = true },
	[crPixelFormat_RedGreen88_Signed] = { .Bytes = 2, .Mipmaps = true },
	[crPixelFormat_RedGreenBlue888_Signed] = { .Bytes = 3, .Mipmaps = true },
	[crPixelFormat_RedGreenBlueAlpha8888_Signed] = { .Bytes = 4, .Mipmaps = true },
	[crPixelFormat_Red8_UInt] = { .Bytes = 1 },
	[crPixelFormat_RedGreen88_UInt] = { .Bytes = 2 },
	[crPixelFormat_RedGreenBlue888_UInt] = { .Bytes = 3 },
	[crPixelFormat_RedGreenBlueAlpha8888_UInt] = { .Bytes = 4 },
	[crPixelFormat_Red8_SInt] = { .Bytes = 1 },
	[crPixelFormat_RedGreen88_SInt] = { .Bytes = 2 },
	[crPixelFormat_RedGreenBlue888_SInt] = { .Bytes = 3 },
	[crPixelFormat_RedGreenBlueAlpha8888_SInt] = { .Bytes = 4 },
	[crPixelFormat_Red16] = { .Bytes = 2, .Mipmaps = true },
	[crPixelFormat_RedGreen16] = { .Bytes = 4, .Mipmaps = true },
	[crPixelFormat_RedGreenBlue16] = { .Bytes = 6, .Mipmaps = true },
	[crPixelFormat_RedGreenBlueAlpha16] = { .Bytes = 8, .Mipmaps = true },
	[crPixelFormat_Red16_Signed] = { .Bytes = 2, .Mipmaps = true },
	[crPixelFormat_RedGreen16_Signed] = { .Bytes = 4, .Mipmaps = true },
	[crPixelFormat_RedGreenBlue16_Signed] = { .Bytes = 6, .Mipmaps = true },
	[crPixelFormat_RedGreenBlueAlpha16_Signed] = { .Bytes = 8, .Mipmaps = true },
	[crPixelFormat_Red16_UInt] = { .Bytes = 2 },
	[crPixelFormat_RedGreen16_UInt] = { .Bytes = 4 },
	[crPixelFormat_RedGreenBlue16_UInt] = { .Bytes = 6 },
	[crPixelFormat_RedGreenBlueAlpha16_UInt] = { .Bytes = 8 },
	[crPixelFormat_Red16_SInt] = { .Bytes = 2 },
	[crPixelFormat_RedGreen16_SInt] = { .Bytes = 4 },
	[crPixelFormat_RedGreenBlue16_SInt] = { .Bytes = 6 },
	[crPixelFormat_RedGreenBlueAlpha16_SInt] = { .Bytes = 8 },
	[crPixelFormat_Red16_Float] = { .Bytes = 2, .Mipmaps = true },
	[crPixelFormat_RedGreen16_Float] = { .Bytes = 4, .Mipmaps = true },
	[crPixelFormat_RedGreenBlue16_Float] = { .Bytes = 6, .Mipmaps = true },
	[crPixelFormat_RedGreenBlueAlpha16_Float] = { .Bytes = 8, .Mipmaps = true },
	[crPixelFormat_Red32_UInt] = { .Bytes = 4 },
	[crPixelFormat_RedGreen32_UInt] = { .Bytes = 8 },
	[crPixelFormat_RedGreenBlue32_UInt] = { .Bytes = 12 },
	[crPixelFormat_RedGreenBlueAlpha32_UInt] = { .Bytes = 16 },
	[crPixelFormat_Red32_SInt] = { .Bytes = 4 },
	[crPixelFormat_RedGreen32_SInt] = { .Bytes = 8 },
	[crPixelFormat_RedGreenBlue32_SInt] = { .Bytes = 12 },
	[crPixelFormat_RedGreenBlueAlpha32_SInt] = { .Bytes = 16 },
	[crPixelFormat_Red32_Float] = { .Bytes = 4, .Mipmaps = true },
	[crPixelFormat_RedGreen32_Float] = { .Bytes = 8, .Mipmaps = true },
	[crPixelFormat_RedGreenBlue32_Float] = { .Bytes = 12, .Mipmaps = true },
	[crPixelFormat_RedGreenBlueAlpha32_Float] = { .Bytes = 16, .Mipmaps = true },
	[crPixelFormat_RedGreenBlueAlpha4444] = { .Bytes = 2, .Mipmaps = true },
	[crPixelFormat_BlueGreenRedAlpha4444] = { .Bytes = 2, .Mipmaps = true },
	[crPixelFormat_AlphaRedGreenBlue4444] = { .Bytes = 2, .Mipmaps = true },
	[crPixelFormat_AlphaBlueGreenRed4444] = { .Bytes = 2, .Mipmaps = true },
	[crPixelFormat_BlueGreenRed565] = { .Bytes = 2, .Mipmaps = true },
	[crPixelFormat_RedGreenBlueAlpha5551] = { .Bytes = 2, .Mipmaps = true },
	[crPixelFormat_BlueGreenRedAlpha5551] = { .Bytes = 2, .Mipmaps = true },
	[crPixelFormat_AlphaRedGreenBlue1555] = { .Bytes = 2, .Mipmaps = true },
	[crPixelFormat_AlphaBlueGreenRed1555] = { .Bytes = 2, .Mipmaps = true },
	[crPixelFormat_AlphaRedGreenBlue2101010] = { .Bytes = 4, .Mipmaps = true },
	[crPixelFormat_AlphaBlueGreenRed2101010] = { .Bytes = 4, .Mipmaps = true },
	[crPixelFormat_AlphaRedGreenBlue2101010_UInt] = { .Bytes = 4 },
	[crPixelFormat_AlphaBlueGreenRed2101010_UInt] = { .Bytes = 4 },
	[crPixelFormat_RedGreenBlue_11_11_10_Float] = { .Bytes = 4, .Mipmaps = true },
	[crPixelFormat_RedGreenBlueE5] = { .Bytes = 4, .Mipmaps = true },
	[crPixelFormat_Alpha8] = { .Bytes = 1, .Mipmaps = true },
	[crPixelFormat_Depth16] = { .Bytes = 2 },
	[crPixelFormat_Depth32_Float] = { .Bytes = 4 },
	[crPixelFormat_Stencil8] = { .Bytes = 1 },
	[crPixelFormat_Depth24_Stencil8] = { .Bytes = 4 },
	[crPixelFormat_Depth32_Float_Stencil8] = { .Bytes = 8 },
	[crPixelFormat_PVRTC1_2BPP] = { .BlockWidth = 8, .BlockHeight = 4, .BlockBytes = 8, .MinBlocksX = 2, .MinBlocksY = 2 },
	[crPixelFormat_PVRTC1_2BPP_SRGB] = { .BlockWidth = 8, .BlockHeight = 4, .BlockBytes = 8, .MinBlocksX = 2, .MinBlocksY = 2 },
	[crPixelFormat_PVRTC1_4BPP] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 8, .MinBlocksX = 2, .MinBlocksY = 2 },
	[crPixelFormat_PVRTC1_4BPP_SRGB] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 8, .MinBlocksX = 2, .MinBlocksY = 2 },
	[crPixelFormat_PVRTC2_2BPP] = { .BlockWidth = 8, .BlockHeight = 4, .BlockBytes = 8, .MinBlocksX = 2, .MinBlocksY = 2 },
	[crPixelFormat_PVRTC2_2BPP_SRGB] = { .BlockWidth = 8, .BlockHeight = 4, .BlockBytes = 8, .MinBlocksX = 2, .MinBlocksY = 2 },
	[crPixelFormat_PVRTC2_4BPP] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 8, .MinBlocksX = 2, .MinBlocksY = 2 },
	[crPixelFormat_PVRTC2_4BPP_SRGB] = { .BlockWidth = 4, .BlockHeight = 4, .BlockBytes = 8, .MinBlocksX = 2, .MinBlocksY = 2 },
	};

int BytesPerPixel ( const crPixelFormat Format )
	{
	if ( Format >= crPixelFormat_Count )
		return 0;
	return ( int ) FormatInfo[Format].Bytes;
	}

bool crPixelFormatAllowsMipmapGeneration ( const crPixelFormat Format )
	{
	return ( Format < crPixelFormat_Count ) && FormatInfo[Format].Mipmaps;
	}

bool crPixelFormatIsCompressed ( const crPixelFormat Format )
	{
	return ( Format < crPixelFormat_Count ) && ( FormatInfo[Format].BlockBytes != 0 );
	}

bool crPixelFormatBlockSize ( const crPixelFormat Format, unsigned *BlockWidth, unsigned *BlockHeight, unsigned *BlockBytes )
	{
	if ( crPixelFormatIsCompressed ( Format ) == false )
		return false;
	*BlockWidth = FormatInfo[Format].BlockWidth;
	*BlockHeight = FormatInfo[Format].BlockHeight;
	*BlockBytes = FormatInfo[Format].BlockBytes;
	return true;
	}

unsigned crPixelFormatDataSize ( const crPixelFormat Format, const unsigned Width, const unsigned Height )
	{
	if ( Format >= crPixelFormat_Count )
		return 0;
	if ( FormatInfo[Format].BlockBytes == 0 )
		return Width * Height * FormatInfo[Format].Bytes;

	unsigned BlocksX = ( Width + FormatInfo[Format].BlockWidth - 1 ) / FormatInfo[Format].BlockWidth;
	unsigned BlocksY = ( Height + FormatInfo[Format].BlockHeight - 1 ) / FormatInfo[Format].BlockHeight;
	const unsigned MinBlocksX = FormatInfo[Format].MinBlocksX == 0 ? 1 : FormatInfo[Format].MinBlocksX;
	const unsigned MinBlocksY = FormatInfo[Format].MinBlocksY == 0 ? 1 : FormatInfo[Format].MinBlocksY;
	if ( BlocksX < MinBlocksX )
		BlocksX = MinBlocksX;
	if ( BlocksY < MinBlocksY )
		BlocksY = MinBlocksY;
	return BlocksX * BlocksY * FormatInfo[Format].BlockBytes;
	}

struct crPixelFormatInfo crGetPixelFormatInfo ( const crPixelFormat Format )
	{
	return FormatInfo[Format];
	}