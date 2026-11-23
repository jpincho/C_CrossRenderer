#include "KTXImageLoader.h"
#include <ktx.h>
#include "gl_format.h"
#include <Platform/Logger.h>

#if defined (PLATFORM_COMPILER_GNU)
#pragma GCC diagnostic pop
#include <fnmatch.h>
#elif defined (PLATFORM_COMPILER_MSVC)
#include <shlwapi.h>
#endif

bool IsFileLoadableWithKTX ( const char *Filename )
	{
	char *SupportedImageFileExtension[] = { "*.ktx",
	                                        "*.ktx2",
	                                      };

	for ( unsigned index = 0; index < sizeof ( SupportedImageFileExtension ) / sizeof ( char * ); ++index )
		{
#if defined (PLATFORM_COMPILER_MSVC)
		if ( PathMatchSpecA ( Filename, SupportedImageFileExtension[index] ) == TRUE )
			return true;
#else
		if ( fnmatch ( SupportedImageFileExtension[index], Filename, 0 ) == 0 )
			return true;
#endif
		}
	return false;
	}


struct KTXFormatMapping
	{
	ktx_uint32_t GLInternalFormat;
	ktx_uint32_t GLType;
	ktx_uint32_t GLTransferFormat;
	ktx_uint32_t VulkanFormat;
	crPixelFormat Format;
	bool SwapRedBlue;
	};

static const struct KTXFormatMapping KTXFormats[] =
	{
		{ .GLInternalFormat = GL_RED, .GLType = GL_UNSIGNED_BYTE, .Format = crPixelFormat_Red8 },
		{ .GLInternalFormat = GL_RG, .GLType = GL_UNSIGNED_BYTE, .Format = crPixelFormat_RedGreen88 },
		{ .GLInternalFormat = GL_RGB, .GLType = GL_UNSIGNED_BYTE, .Format = crPixelFormat_RedGreenBlue888 },
		{ .GLInternalFormat = GL_RGBA, .GLType = GL_UNSIGNED_BYTE, .Format = crPixelFormat_RedGreenBlueAlpha8888 },
		{ .GLInternalFormat = GL_BGR, .GLType = GL_UNSIGNED_BYTE, .Format = crPixelFormat_RedGreenBlue888, .SwapRedBlue = true },
		{ .GLInternalFormat = GL_BGRA, .GLType = GL_UNSIGNED_BYTE, .Format = crPixelFormat_RedGreenBlueAlpha8888, .SwapRedBlue = true },
		{ .GLInternalFormat = GL_R8, .GLType = GL_UNSIGNED_BYTE, .VulkanFormat = VK_FORMAT_R8_UNORM, .Format = crPixelFormat_Red8 },
		{ .GLInternalFormat = GL_R8_SNORM, .GLType = GL_BYTE, .VulkanFormat = VK_FORMAT_R8_SNORM, .Format = crPixelFormat_Red8_Signed },
		{ .GLInternalFormat = GL_R8UI, .GLType = GL_UNSIGNED_BYTE, .VulkanFormat = VK_FORMAT_R8_UINT, .Format = crPixelFormat_Red8_UInt },
		{ .GLInternalFormat = GL_R8I, .GLType = GL_BYTE, .VulkanFormat = VK_FORMAT_R8_SINT, .Format = crPixelFormat_Red8_SInt },
		{ .GLInternalFormat = GL_SR8, .GLType = GL_UNSIGNED_BYTE, .VulkanFormat = VK_FORMAT_R8_SRGB, .Format = crPixelFormat_Red8_SRGB },
		{ .GLInternalFormat = GL_RG8, .GLType = GL_UNSIGNED_BYTE, .VulkanFormat = VK_FORMAT_R8G8_UNORM, .Format = crPixelFormat_RedGreen88 },
		{ .GLInternalFormat = GL_RG8_SNORM, .GLType = GL_BYTE, .VulkanFormat = VK_FORMAT_R8G8_SNORM, .Format = crPixelFormat_RedGreen88_Signed },
		{ .GLInternalFormat = GL_RG8UI, .GLType = GL_UNSIGNED_BYTE, .VulkanFormat = VK_FORMAT_R8G8_UINT, .Format = crPixelFormat_RedGreen88_UInt },
		{ .GLInternalFormat = GL_RG8I, .GLType = GL_BYTE, .VulkanFormat = VK_FORMAT_R8G8_SINT, .Format = crPixelFormat_RedGreen88_SInt },
		{ .GLInternalFormat = GL_SRG8, .GLType = GL_UNSIGNED_BYTE, .VulkanFormat = VK_FORMAT_R8G8_SRGB, .Format = crPixelFormat_RedGreen88_SRGB },
		{ .GLInternalFormat = GL_RGB8, .GLType = GL_UNSIGNED_BYTE, .VulkanFormat = VK_FORMAT_R8G8B8_UNORM, .Format = crPixelFormat_RedGreenBlue888 },
		{ .GLInternalFormat = GL_RGB8_SNORM, .GLType = GL_BYTE, .VulkanFormat = VK_FORMAT_R8G8B8_SNORM, .Format = crPixelFormat_RedGreenBlue888_Signed },
		{ .GLInternalFormat = GL_RGB8UI, .GLType = GL_UNSIGNED_BYTE, .VulkanFormat = VK_FORMAT_R8G8B8_UINT, .Format = crPixelFormat_RedGreenBlue888_UInt },
		{ .GLInternalFormat = GL_RGB8I, .GLType = GL_BYTE, .VulkanFormat = VK_FORMAT_R8G8B8_SINT, .Format = crPixelFormat_RedGreenBlue888_SInt },
		{ .GLInternalFormat = GL_SRGB8, .GLType = GL_UNSIGNED_BYTE, .VulkanFormat = VK_FORMAT_R8G8B8_SRGB, .Format = crPixelFormat_RedGreenBlue888_SRGB },
		{ .VulkanFormat = VK_FORMAT_B8G8R8_UNORM, .Format = crPixelFormat_RedGreenBlue888, .SwapRedBlue = true },
		{ .VulkanFormat = VK_FORMAT_B8G8R8_SNORM, .Format = crPixelFormat_RedGreenBlue888_Signed, .SwapRedBlue = true },
		{ .VulkanFormat = VK_FORMAT_B8G8R8_UINT, .Format = crPixelFormat_RedGreenBlue888_UInt, .SwapRedBlue = true },
		{ .VulkanFormat = VK_FORMAT_B8G8R8_SINT, .Format = crPixelFormat_RedGreenBlue888_SInt, .SwapRedBlue = true },
		{ .VulkanFormat = VK_FORMAT_B8G8R8_SRGB, .Format = crPixelFormat_RedGreenBlue888_SRGB, .SwapRedBlue = true },
		{ .GLInternalFormat = GL_RGBA8, .GLType = GL_UNSIGNED_BYTE, .VulkanFormat = VK_FORMAT_R8G8B8A8_UNORM, .Format = crPixelFormat_RedGreenBlueAlpha8888 },
		{ .GLInternalFormat = GL_RGBA8_SNORM, .GLType = GL_BYTE, .VulkanFormat = VK_FORMAT_R8G8B8A8_SNORM, .Format = crPixelFormat_RedGreenBlueAlpha8888_Signed },
		{ .GLInternalFormat = GL_RGBA8UI, .GLType = GL_UNSIGNED_BYTE, .VulkanFormat = VK_FORMAT_R8G8B8A8_UINT, .Format = crPixelFormat_RedGreenBlueAlpha8888_UInt },
		{ .GLInternalFormat = GL_RGBA8I, .GLType = GL_BYTE, .VulkanFormat = VK_FORMAT_R8G8B8A8_SINT, .Format = crPixelFormat_RedGreenBlueAlpha8888_SInt },
		{ .GLInternalFormat = GL_SRGB8_ALPHA8, .GLType = GL_UNSIGNED_BYTE, .VulkanFormat = VK_FORMAT_R8G8B8A8_SRGB, .Format = crPixelFormat_RedGreenBlueAlpha8888_SRGB },
		{ .VulkanFormat = VK_FORMAT_B8G8R8A8_UNORM, .Format = crPixelFormat_RedGreenBlueAlpha8888, .SwapRedBlue = true },
		{ .VulkanFormat = VK_FORMAT_B8G8R8A8_SNORM, .Format = crPixelFormat_RedGreenBlueAlpha8888_Signed, .SwapRedBlue = true },
		{ .VulkanFormat = VK_FORMAT_B8G8R8A8_UINT, .Format = crPixelFormat_RedGreenBlueAlpha8888_UInt, .SwapRedBlue = true },
		{ .VulkanFormat = VK_FORMAT_B8G8R8A8_SINT, .Format = crPixelFormat_RedGreenBlueAlpha8888_SInt, .SwapRedBlue = true },
		{ .VulkanFormat = VK_FORMAT_B8G8R8A8_SRGB, .Format = crPixelFormat_RedGreenBlueAlpha8888_SRGB, .SwapRedBlue = true },
		{ .VulkanFormat = VK_FORMAT_A8B8G8R8_UNORM_PACK32, .Format = crPixelFormat_RedGreenBlueAlpha8888 },
		{ .VulkanFormat = VK_FORMAT_A8B8G8R8_SNORM_PACK32, .Format = crPixelFormat_RedGreenBlueAlpha8888_Signed },
		{ .VulkanFormat = VK_FORMAT_A8B8G8R8_UINT_PACK32, .Format = crPixelFormat_RedGreenBlueAlpha8888_UInt },
		{ .VulkanFormat = VK_FORMAT_A8B8G8R8_SINT_PACK32, .Format = crPixelFormat_RedGreenBlueAlpha8888_SInt },
		{ .VulkanFormat = VK_FORMAT_A8B8G8R8_SRGB_PACK32, .Format = crPixelFormat_RedGreenBlueAlpha8888_SRGB },
		{ .GLInternalFormat = GL_R16, .GLType = GL_UNSIGNED_SHORT, .VulkanFormat = VK_FORMAT_R16_UNORM, .Format = crPixelFormat_Red16 },
		{ .GLInternalFormat = GL_R16_SNORM, .GLType = GL_SHORT, .VulkanFormat = VK_FORMAT_R16_SNORM, .Format = crPixelFormat_Red16_Signed },
		{ .GLInternalFormat = GL_R16UI, .GLType = GL_UNSIGNED_SHORT, .VulkanFormat = VK_FORMAT_R16_UINT, .Format = crPixelFormat_Red16_UInt },
		{ .GLInternalFormat = GL_R16I, .GLType = GL_SHORT, .VulkanFormat = VK_FORMAT_R16_SINT, .Format = crPixelFormat_Red16_SInt },
		{ .GLInternalFormat = GL_R16F, .GLType = GL_HALF_FLOAT, .VulkanFormat = VK_FORMAT_R16_SFLOAT, .Format = crPixelFormat_Red16_Float },
		{ .GLInternalFormat = GL_RG16, .GLType = GL_UNSIGNED_SHORT, .VulkanFormat = VK_FORMAT_R16G16_UNORM, .Format = crPixelFormat_RedGreen16 },
		{ .GLInternalFormat = GL_RG16_SNORM, .GLType = GL_SHORT, .VulkanFormat = VK_FORMAT_R16G16_SNORM, .Format = crPixelFormat_RedGreen16_Signed },
		{ .GLInternalFormat = GL_RG16UI, .GLType = GL_UNSIGNED_SHORT, .VulkanFormat = VK_FORMAT_R16G16_UINT, .Format = crPixelFormat_RedGreen16_UInt },
		{ .GLInternalFormat = GL_RG16I, .GLType = GL_SHORT, .VulkanFormat = VK_FORMAT_R16G16_SINT, .Format = crPixelFormat_RedGreen16_SInt },
		{ .GLInternalFormat = GL_RG16F, .GLType = GL_HALF_FLOAT, .VulkanFormat = VK_FORMAT_R16G16_SFLOAT, .Format = crPixelFormat_RedGreen16_Float },
		{ .GLInternalFormat = GL_RGB16, .GLType = GL_UNSIGNED_SHORT, .VulkanFormat = VK_FORMAT_R16G16B16_UNORM, .Format = crPixelFormat_RedGreenBlue16 },
		{ .GLInternalFormat = GL_RGB16_SNORM, .GLType = GL_SHORT, .VulkanFormat = VK_FORMAT_R16G16B16_SNORM, .Format = crPixelFormat_RedGreenBlue16_Signed },
		{ .GLInternalFormat = GL_RGB16UI, .GLType = GL_UNSIGNED_SHORT, .VulkanFormat = VK_FORMAT_R16G16B16_UINT, .Format = crPixelFormat_RedGreenBlue16_UInt },
		{ .GLInternalFormat = GL_RGB16I, .GLType = GL_SHORT, .VulkanFormat = VK_FORMAT_R16G16B16_SINT, .Format = crPixelFormat_RedGreenBlue16_SInt },
		{ .GLInternalFormat = GL_RGB16F, .GLType = GL_HALF_FLOAT, .VulkanFormat = VK_FORMAT_R16G16B16_SFLOAT, .Format = crPixelFormat_RedGreenBlue16_Float },
		{ .GLInternalFormat = GL_RGBA16, .GLType = GL_UNSIGNED_SHORT, .VulkanFormat = VK_FORMAT_R16G16B16A16_UNORM, .Format = crPixelFormat_RedGreenBlueAlpha16 },
		{ .GLInternalFormat = GL_RGBA16_SNORM, .GLType = GL_SHORT, .VulkanFormat = VK_FORMAT_R16G16B16A16_SNORM, .Format = crPixelFormat_RedGreenBlueAlpha16_Signed },
		{ .GLInternalFormat = GL_RGBA16UI, .GLType = GL_UNSIGNED_SHORT, .VulkanFormat = VK_FORMAT_R16G16B16A16_UINT, .Format = crPixelFormat_RedGreenBlueAlpha16_UInt },
		{ .GLInternalFormat = GL_RGBA16I, .GLType = GL_SHORT, .VulkanFormat = VK_FORMAT_R16G16B16A16_SINT, .Format = crPixelFormat_RedGreenBlueAlpha16_SInt },
		{ .GLInternalFormat = GL_RGBA16F, .GLType = GL_HALF_FLOAT, .VulkanFormat = VK_FORMAT_R16G16B16A16_SFLOAT, .Format = crPixelFormat_RedGreenBlueAlpha16_Float },
		{ .GLInternalFormat = GL_R32UI, .GLType = GL_UNSIGNED_INT, .VulkanFormat = VK_FORMAT_R32_UINT, .Format = crPixelFormat_Red32_UInt },
		{ .GLInternalFormat = GL_R32I, .GLType = GL_INT, .VulkanFormat = VK_FORMAT_R32_SINT, .Format = crPixelFormat_Red32_SInt },
		{ .GLInternalFormat = GL_R32F, .GLType = GL_FLOAT, .VulkanFormat = VK_FORMAT_R32_SFLOAT, .Format = crPixelFormat_Red32_Float },
		{ .GLInternalFormat = GL_RG32UI, .GLType = GL_UNSIGNED_INT, .VulkanFormat = VK_FORMAT_R32G32_UINT, .Format = crPixelFormat_RedGreen32_UInt },
		{ .GLInternalFormat = GL_RG32I, .GLType = GL_INT, .VulkanFormat = VK_FORMAT_R32G32_SINT, .Format = crPixelFormat_RedGreen32_SInt },
		{ .GLInternalFormat = GL_RG32F, .GLType = GL_FLOAT, .VulkanFormat = VK_FORMAT_R32G32_SFLOAT, .Format = crPixelFormat_RedGreen32_Float },
		{ .GLInternalFormat = GL_RGB32UI, .GLType = GL_UNSIGNED_INT, .VulkanFormat = VK_FORMAT_R32G32B32_UINT, .Format = crPixelFormat_RedGreenBlue32_UInt },
		{ .GLInternalFormat = GL_RGB32I, .GLType = GL_INT, .VulkanFormat = VK_FORMAT_R32G32B32_SINT, .Format = crPixelFormat_RedGreenBlue32_SInt },
		{ .GLInternalFormat = GL_RGB32F, .GLType = GL_FLOAT, .VulkanFormat = VK_FORMAT_R32G32B32_SFLOAT, .Format = crPixelFormat_RedGreenBlue32_Float },
		{ .GLInternalFormat = GL_RGBA32UI, .GLType = GL_UNSIGNED_INT, .VulkanFormat = VK_FORMAT_R32G32B32A32_UINT, .Format = crPixelFormat_RedGreenBlueAlpha32_UInt },
		{ .GLInternalFormat = GL_RGBA32I, .GLType = GL_INT, .VulkanFormat = VK_FORMAT_R32G32B32A32_SINT, .Format = crPixelFormat_RedGreenBlueAlpha32_SInt },
		{ .GLInternalFormat = GL_RGBA32F, .GLType = GL_FLOAT, .VulkanFormat = VK_FORMAT_R32G32B32A32_SFLOAT, .Format = crPixelFormat_RedGreenBlueAlpha32_Float },
		{ .GLInternalFormat = GL_RGBA4, .GLType = GL_UNSIGNED_SHORT_4_4_4_4, .GLTransferFormat = GL_RGBA, .VulkanFormat = VK_FORMAT_R4G4B4A4_UNORM_PACK16, .Format = crPixelFormat_RedGreenBlueAlpha4444 },
		{ .GLInternalFormat = GL_RGBA4, .GLType = GL_UNSIGNED_SHORT_4_4_4_4, .GLTransferFormat = GL_BGRA, .VulkanFormat = VK_FORMAT_B4G4R4A4_UNORM_PACK16, .Format = crPixelFormat_BlueGreenRedAlpha4444 },
		{ .GLInternalFormat = GL_RGBA4, .GLType = GL_UNSIGNED_SHORT_4_4_4_4_REV, .GLTransferFormat = GL_BGRA, .VulkanFormat = VK_FORMAT_A4R4G4B4_UNORM_PACK16, .Format = crPixelFormat_AlphaRedGreenBlue4444 },
		{ .GLInternalFormat = GL_RGBA4, .GLType = GL_UNSIGNED_SHORT_4_4_4_4_REV, .GLTransferFormat = GL_RGBA, .VulkanFormat = VK_FORMAT_A4B4G4R4_UNORM_PACK16, .Format = crPixelFormat_AlphaBlueGreenRed4444 },
		{ .GLInternalFormat = GL_RGB565, .GLType = GL_UNSIGNED_SHORT_5_6_5, .VulkanFormat = VK_FORMAT_R5G6B5_UNORM_PACK16, .Format = crPixelFormat_RedGreenBlue565 },
		{ .GLInternalFormat = GL_RGB, .GLType = GL_UNSIGNED_SHORT_5_6_5, .Format = crPixelFormat_RedGreenBlue565 },
		{ .GLInternalFormat = GL_RGB565, .GLType = GL_UNSIGNED_SHORT_5_6_5_REV, .VulkanFormat = VK_FORMAT_B5G6R5_UNORM_PACK16, .Format = crPixelFormat_BlueGreenRed565 },
		{ .GLInternalFormat = GL_RGB, .GLType = GL_UNSIGNED_SHORT_5_6_5_REV, .Format = crPixelFormat_BlueGreenRed565 },
		{ .GLInternalFormat = GL_RGB5_A1, .GLType = GL_UNSIGNED_SHORT_5_5_5_1, .GLTransferFormat = GL_RGBA, .VulkanFormat = VK_FORMAT_R5G5B5A1_UNORM_PACK16, .Format = crPixelFormat_RedGreenBlueAlpha5551 },
		{ .GLInternalFormat = GL_RGB5_A1, .GLType = GL_UNSIGNED_SHORT_5_5_5_1, .GLTransferFormat = GL_BGRA, .VulkanFormat = VK_FORMAT_B5G5R5A1_UNORM_PACK16, .Format = crPixelFormat_BlueGreenRedAlpha5551 },
		{ .GLInternalFormat = GL_RGB5_A1, .GLType = GL_UNSIGNED_SHORT_1_5_5_5_REV, .GLTransferFormat = GL_BGRA, .VulkanFormat = VK_FORMAT_A1R5G5B5_UNORM_PACK16, .Format = crPixelFormat_AlphaRedGreenBlue1555 },
		{ .GLInternalFormat = GL_RGB5_A1, .GLType = GL_UNSIGNED_SHORT_1_5_5_5_REV, .GLTransferFormat = GL_RGBA, .VulkanFormat = VK_FORMAT_A1B5G5R5_UNORM_PACK16_KHR, .Format = crPixelFormat_AlphaBlueGreenRed1555 },
		{ .GLInternalFormat = GL_RGB10_A2, .GLType = GL_UNSIGNED_INT_2_10_10_10_REV, .GLTransferFormat = GL_BGRA, .VulkanFormat = VK_FORMAT_A2R10G10B10_UNORM_PACK32, .Format = crPixelFormat_AlphaRedGreenBlue2101010 },
		{ .GLInternalFormat = GL_RGB10_A2, .GLType = GL_UNSIGNED_INT_2_10_10_10_REV, .GLTransferFormat = GL_RGBA, .VulkanFormat = VK_FORMAT_A2B10G10R10_UNORM_PACK32, .Format = crPixelFormat_AlphaBlueGreenRed2101010 },
		{ .GLInternalFormat = GL_RGB10_A2UI, .GLType = GL_UNSIGNED_INT_2_10_10_10_REV, .GLTransferFormat = GL_BGRA_INTEGER, .VulkanFormat = VK_FORMAT_A2R10G10B10_UINT_PACK32, .Format = crPixelFormat_AlphaRedGreenBlue2101010_UInt },
		{ .GLInternalFormat = GL_RGB10_A2UI, .GLType = GL_UNSIGNED_INT_2_10_10_10_REV, .GLTransferFormat = GL_RGBA_INTEGER, .VulkanFormat = VK_FORMAT_A2B10G10R10_UINT_PACK32, .Format = crPixelFormat_AlphaBlueGreenRed2101010_UInt },
		{ .GLInternalFormat = GL_R11F_G11F_B10F, .GLType = GL_UNSIGNED_INT_10F_11F_11F_REV, .VulkanFormat = VK_FORMAT_B10G11R11_UFLOAT_PACK32, .Format = crPixelFormat_RedGreenBlue_11_11_10_Float },
		{ .GLInternalFormat = GL_RGB9_E5, .GLType = GL_UNSIGNED_INT_5_9_9_9_REV, .VulkanFormat = VK_FORMAT_E5B9G9R9_UFLOAT_PACK32, .Format = crPixelFormat_RedGreenBlueE5 },
		{ .GLInternalFormat = GL_ALPHA8, .GLType = GL_UNSIGNED_BYTE, .VulkanFormat = VK_FORMAT_A8_UNORM_KHR, .Format = crPixelFormat_Alpha8 },
		{ .GLInternalFormat = GL_ALPHA, .GLType = GL_UNSIGNED_BYTE, .Format = crPixelFormat_Alpha8 },
		{ .GLInternalFormat = GL_DEPTH_COMPONENT16, .GLType = GL_UNSIGNED_SHORT, .VulkanFormat = VK_FORMAT_D16_UNORM, .Format = crPixelFormat_Depth16 },
		{ .GLInternalFormat = GL_DEPTH_COMPONENT32F, .GLType = GL_FLOAT, .VulkanFormat = VK_FORMAT_D32_SFLOAT, .Format = crPixelFormat_Depth32_Float },
		{ .GLInternalFormat = GL_STENCIL_INDEX8, .GLType = GL_UNSIGNED_BYTE, .VulkanFormat = VK_FORMAT_S8_UINT, .Format = crPixelFormat_Stencil8 },
		{ .GLInternalFormat = GL_DEPTH24_STENCIL8, .GLType = GL_UNSIGNED_INT_24_8, .VulkanFormat = VK_FORMAT_D24_UNORM_S8_UINT, .Format = crPixelFormat_Depth24_Stencil8 },
		{ .GLInternalFormat = GL_DEPTH32F_STENCIL8, .GLType = GL_FLOAT_32_UNSIGNED_INT_24_8_REV, .VulkanFormat = VK_FORMAT_D32_SFLOAT_S8_UINT, .Format = crPixelFormat_Depth32_Float_Stencil8 },
		{ .GLInternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT, .VulkanFormat = VK_FORMAT_BC1_RGB_UNORM_BLOCK, .Format = crPixelFormat_BC1_RGB },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, .VulkanFormat = VK_FORMAT_BC1_RGB_SRGB_BLOCK, .Format = crPixelFormat_BC1_RGB_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, .VulkanFormat = VK_FORMAT_BC1_RGBA_UNORM_BLOCK, .Format = crPixelFormat_BC1_RGBA },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, .VulkanFormat = VK_FORMAT_BC1_RGBA_SRGB_BLOCK, .Format = crPixelFormat_BC1_RGBA_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, .VulkanFormat = VK_FORMAT_BC2_UNORM_BLOCK, .Format = crPixelFormat_BC2 },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, .VulkanFormat = VK_FORMAT_BC2_SRGB_BLOCK, .Format = crPixelFormat_BC2_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, .VulkanFormat = VK_FORMAT_BC3_UNORM_BLOCK, .Format = crPixelFormat_BC3 },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, .VulkanFormat = VK_FORMAT_BC3_SRGB_BLOCK, .Format = crPixelFormat_BC3_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RED_RGTC1, .VulkanFormat = VK_FORMAT_BC4_UNORM_BLOCK, .Format = crPixelFormat_BC4 },
		{ .GLInternalFormat = GL_COMPRESSED_SIGNED_RED_RGTC1, .VulkanFormat = VK_FORMAT_BC4_SNORM_BLOCK, .Format = crPixelFormat_BC4_Signed },
		{ .GLInternalFormat = GL_COMPRESSED_RG_RGTC2, .VulkanFormat = VK_FORMAT_BC5_UNORM_BLOCK, .Format = crPixelFormat_BC5 },
		{ .GLInternalFormat = GL_COMPRESSED_SIGNED_RG_RGTC2, .VulkanFormat = VK_FORMAT_BC5_SNORM_BLOCK, .Format = crPixelFormat_BC5_Signed },
		{ .GLInternalFormat = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT, .VulkanFormat = VK_FORMAT_BC6H_UFLOAT_BLOCK, .Format = crPixelFormat_BC6H },
		{ .GLInternalFormat = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT, .VulkanFormat = VK_FORMAT_BC6H_SFLOAT_BLOCK, .Format = crPixelFormat_BC6H_Signed },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_BPTC_UNORM, .VulkanFormat = VK_FORMAT_BC7_UNORM_BLOCK, .Format = crPixelFormat_BC7 },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, .VulkanFormat = VK_FORMAT_BC7_SRGB_BLOCK, .Format = crPixelFormat_BC7_SRGB },
		{ .GLInternalFormat = GL_ETC1_RGB8_OES, .Format = crPixelFormat_ETC2_RGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGB8_ETC2, .VulkanFormat = VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK, .Format = crPixelFormat_ETC2_RGB },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB8_ETC2, .VulkanFormat = VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK, .Format = crPixelFormat_ETC2_RGB_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, .VulkanFormat = VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK, .Format = crPixelFormat_ETC2_RGBA1 },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, .VulkanFormat = VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK, .Format = crPixelFormat_ETC2_RGBA1_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA8_ETC2_EAC, .VulkanFormat = VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK, .Format = crPixelFormat_ETC2_RGBA },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, .VulkanFormat = VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK, .Format = crPixelFormat_ETC2_RGBA_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_R11_EAC, .VulkanFormat = VK_FORMAT_EAC_R11_UNORM_BLOCK, .Format = crPixelFormat_EAC_R11 },
		{ .GLInternalFormat = GL_COMPRESSED_SIGNED_R11_EAC, .VulkanFormat = VK_FORMAT_EAC_R11_SNORM_BLOCK, .Format = crPixelFormat_EAC_R11_Signed },
		{ .GLInternalFormat = GL_COMPRESSED_RG11_EAC, .VulkanFormat = VK_FORMAT_EAC_R11G11_UNORM_BLOCK, .Format = crPixelFormat_EAC_RG11 },
		{ .GLInternalFormat = GL_COMPRESSED_SIGNED_RG11_EAC, .VulkanFormat = VK_FORMAT_EAC_R11G11_SNORM_BLOCK, .Format = crPixelFormat_EAC_RG11_Signed },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_ASTC_4x4_KHR, .VulkanFormat = VK_FORMAT_ASTC_4x4_UNORM_BLOCK, .Format = crPixelFormat_ASTC_4x4 },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, .VulkanFormat = VK_FORMAT_ASTC_4x4_SRGB_BLOCK, .Format = crPixelFormat_ASTC_4x4_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_ASTC_5x4_KHR, .VulkanFormat = VK_FORMAT_ASTC_5x4_UNORM_BLOCK, .Format = crPixelFormat_ASTC_5x4 },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR, .VulkanFormat = VK_FORMAT_ASTC_5x4_SRGB_BLOCK, .Format = crPixelFormat_ASTC_5x4_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_ASTC_5x5_KHR, .VulkanFormat = VK_FORMAT_ASTC_5x5_UNORM_BLOCK, .Format = crPixelFormat_ASTC_5x5 },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR, .VulkanFormat = VK_FORMAT_ASTC_5x5_SRGB_BLOCK, .Format = crPixelFormat_ASTC_5x5_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_ASTC_6x5_KHR, .VulkanFormat = VK_FORMAT_ASTC_6x5_UNORM_BLOCK, .Format = crPixelFormat_ASTC_6x5 },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR, .VulkanFormat = VK_FORMAT_ASTC_6x5_SRGB_BLOCK, .Format = crPixelFormat_ASTC_6x5_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_ASTC_6x6_KHR, .VulkanFormat = VK_FORMAT_ASTC_6x6_UNORM_BLOCK, .Format = crPixelFormat_ASTC_6x6 },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR, .VulkanFormat = VK_FORMAT_ASTC_6x6_SRGB_BLOCK, .Format = crPixelFormat_ASTC_6x6_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_ASTC_8x5_KHR, .VulkanFormat = VK_FORMAT_ASTC_8x5_UNORM_BLOCK, .Format = crPixelFormat_ASTC_8x5 },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR, .VulkanFormat = VK_FORMAT_ASTC_8x5_SRGB_BLOCK, .Format = crPixelFormat_ASTC_8x5_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_ASTC_8x6_KHR, .VulkanFormat = VK_FORMAT_ASTC_8x6_UNORM_BLOCK, .Format = crPixelFormat_ASTC_8x6 },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR, .VulkanFormat = VK_FORMAT_ASTC_8x6_SRGB_BLOCK, .Format = crPixelFormat_ASTC_8x6_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_ASTC_8x8_KHR, .VulkanFormat = VK_FORMAT_ASTC_8x8_UNORM_BLOCK, .Format = crPixelFormat_ASTC_8x8 },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR, .VulkanFormat = VK_FORMAT_ASTC_8x8_SRGB_BLOCK, .Format = crPixelFormat_ASTC_8x8_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_ASTC_10x5_KHR, .VulkanFormat = VK_FORMAT_ASTC_10x5_UNORM_BLOCK, .Format = crPixelFormat_ASTC_10x5 },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR, .VulkanFormat = VK_FORMAT_ASTC_10x5_SRGB_BLOCK, .Format = crPixelFormat_ASTC_10x5_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_ASTC_10x6_KHR, .VulkanFormat = VK_FORMAT_ASTC_10x6_UNORM_BLOCK, .Format = crPixelFormat_ASTC_10x6 },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR, .VulkanFormat = VK_FORMAT_ASTC_10x6_SRGB_BLOCK, .Format = crPixelFormat_ASTC_10x6_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_ASTC_10x8_KHR, .VulkanFormat = VK_FORMAT_ASTC_10x8_UNORM_BLOCK, .Format = crPixelFormat_ASTC_10x8 },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR, .VulkanFormat = VK_FORMAT_ASTC_10x8_SRGB_BLOCK, .Format = crPixelFormat_ASTC_10x8_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_ASTC_10x10_KHR, .VulkanFormat = VK_FORMAT_ASTC_10x10_UNORM_BLOCK, .Format = crPixelFormat_ASTC_10x10 },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR, .VulkanFormat = VK_FORMAT_ASTC_10x10_SRGB_BLOCK, .Format = crPixelFormat_ASTC_10x10_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_ASTC_12x10_KHR, .VulkanFormat = VK_FORMAT_ASTC_12x10_UNORM_BLOCK, .Format = crPixelFormat_ASTC_12x10 },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR, .VulkanFormat = VK_FORMAT_ASTC_12x10_SRGB_BLOCK, .Format = crPixelFormat_ASTC_12x10_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_ASTC_12x12_KHR, .VulkanFormat = VK_FORMAT_ASTC_12x12_UNORM_BLOCK, .Format = crPixelFormat_ASTC_12x12 },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR, .VulkanFormat = VK_FORMAT_ASTC_12x12_SRGB_BLOCK, .Format = crPixelFormat_ASTC_12x12_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG, .VulkanFormat = VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG, .Format = crPixelFormat_PVRTC1_2BPP },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, .VulkanFormat = VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG, .Format = crPixelFormat_PVRTC1_4BPP },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG, .VulkanFormat = VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG, .Format = crPixelFormat_PVRTC2_2BPP },
		{ .GLInternalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG, .VulkanFormat = VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG, .Format = crPixelFormat_PVRTC2_4BPP },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT, .VulkanFormat = VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG, .Format = crPixelFormat_PVRTC1_2BPP_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT, .VulkanFormat = VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG, .Format = crPixelFormat_PVRTC1_4BPP_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG, .VulkanFormat = VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG, .Format = crPixelFormat_PVRTC2_2BPP_SRGB },
		{ .GLInternalFormat = GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG, .VulkanFormat = VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG, .Format = crPixelFormat_PVRTC2_4BPP_SRGB },
	};

static bool EntryMatchesGL ( const struct KTXFormatMapping *Entry, const ktx_uint32_t InternalFormat, const ktx_uint32_t Type, const ktx_uint32_t TransferFormat )
	{
	if ( ( Entry->GLInternalFormat == 0 ) || ( Entry->GLInternalFormat != InternalFormat ) )
		return false;
	if ( Type == 0 )
		return Entry->GLType == 0;
	if ( ( Entry->GLType != 0 ) && ( Entry->GLType != Type ) )
		return false;
	if ( ( Entry->GLTransferFormat != 0 ) && ( Entry->GLTransferFormat != TransferFormat ) )
		return false;
	return true;
	}

static int EntrySpecificity ( const struct KTXFormatMapping *Entry )
	{
	return ( Entry->GLType != 0 ) + ( Entry->GLTransferFormat != 0 );
	}

static bool FindGLFormat ( const ktx_uint32_t InternalFormat, const ktx_uint32_t Type, const ktx_uint32_t TransferFormat, crPixelFormat *Format, bool *SwapRedBlue )
	{
	int BestScore = -1;
	bool Found = false;
	for ( unsigned Index = 0; Index < sizeof ( KTXFormats ) / sizeof ( KTXFormats[0] ); ++Index )
		{
		if ( EntryMatchesGL ( &KTXFormats[Index], InternalFormat, Type, TransferFormat ) == false )
			continue;
		const int Score = EntrySpecificity ( &KTXFormats[Index] );
		if ( ( Found == false ) || ( Score > BestScore ) )
			{
			Found = true;
			BestScore = Score;
			*Format = KTXFormats[Index].Format;
			*SwapRedBlue = KTXFormats[Index].SwapRedBlue;
			}
		}
	return Found;
	}

static bool FindVulkanFormat ( const ktx_uint32_t Value, crPixelFormat *Format, bool *SwapRedBlue )
	{
	if ( ( Value >= VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK ) && ( Value <= VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK ) )
		{
		*Format = ( crPixelFormat ) ( crPixelFormat_ASTC_4x4 + ( int ) ( Value - VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK ) * 2 );
		*SwapRedBlue = false;
		return true;
		}

	for ( unsigned Index = 0; Index < sizeof ( KTXFormats ) / sizeof ( KTXFormats[0] ); ++Index )
		{
		if ( ( KTXFormats[Index].VulkanFormat != 0 ) && ( KTXFormats[Index].VulkanFormat == Value ) )
			{
			*Format = KTXFormats[Index].Format;
			*SwapRedBlue = KTXFormats[Index].SwapRedBlue;
			return true;
			}
		}
	return false;
	}

static bool IsByteComponentType ( const ktx_uint32_t Type )
	{
	return ( Type == GL_BYTE ) || ( Type == GL_UNSIGNED_BYTE );
	}

static bool IsBlueFirstFormat ( const ktx_uint32_t TransferFormat )
	{
	return ( TransferFormat == GL_BGR ) || ( TransferFormat == GL_BGRA ) ||
	       ( TransferFormat == GL_BGR_INTEGER ) || ( TransferFormat == GL_BGRA_INTEGER );
	}

static bool TranslateLoadedKTXFormat ( ktxTexture *Texture, crPixelFormat *Format, bool *SwapRedBlue )
	{
	*Format = crPixelFormat_Invalid;
	*SwapRedBlue = false;

	switch ( Texture->classId )
		{
		case ktxTexture1_c:
			{
			ktxTexture1 *Texture1 = ( ktxTexture1 * ) Texture;
			if ( FindGLFormat ( Texture1->glInternalformat, Texture1->glType, Texture1->glFormat, Format, SwapRedBlue ) == false )
				{
				if ( FindGLFormat ( Texture1->glFormat, Texture1->glType, 0, Format, SwapRedBlue ) == false )
					return false;
				}
			struct crPixelFormatInfo PixelFormatInfo = crGetPixelFormatInfo ( *Format );
			if ( ( *SwapRedBlue == false ) && ( IsByteComponentType ( Texture1->glType ) ) && ( IsBlueFirstFormat ( Texture1->glFormat ) ) && ( PixelFormatInfo.Bytes >= 3 ) )
				*SwapRedBlue = true;
			return true;
			}
		case ktxTexture2_c:
			{
			ktxTexture2 *Texture2 = ( ktxTexture2 * ) Texture;
			return FindVulkanFormat ( Texture2->vkFormat, Format, SwapRedBlue );
			}
		default:
			return false;
		}
	}

static void CopyUncompressedKTXImage ( uint8_t *Destination, const uint8_t *Source, const unsigned Width, const unsigned Height, const unsigned RowPitch, const unsigned ElementSize, const bool SwapRedBlue )
	{
	const unsigned TightPitch = Width * ElementSize;
	for ( unsigned Row = 0; Row < Height; ++Row )
		{
		uint8_t *DestinationRow = Destination + ( size_t ) Row * TightPitch;
		memcpy ( DestinationRow, Source + ( size_t ) Row * RowPitch, TightPitch );
		if ( SwapRedBlue )
			{
			for ( unsigned Column = 0; Column < Width; ++Column )
				{
				uint8_t *Pixel = DestinationRow + ( size_t ) Column * ElementSize;
				uint8_t Temporary = Pixel[0];
				Pixel[0] = Pixel[2];
				Pixel[2] = Temporary;
				}
			}
		}
	}

static bool LoadTextureFromKTXTexture ( ktxTexture *Texture, crTextureDescriptor *Descriptor )
	{
	uint8_t *Pixels = NULL;
	KTX_error_code Result;
	if ( ( Texture->isArray == KTX_TRUE ) || ( Texture->isCubemap == KTX_TRUE ) ||
	        ( Texture->numDimensions != 2 ) || ( Texture->numFaces != 1 ) ||
	        ( Texture->baseWidth == 0 ) || ( Texture->baseHeight == 0 ) )
		{
		LOG_ERROR ( "Texture is not a 2D image" );
		goto OnError;
		}

	if ( ktxTexture_NeedsTranscoding ( Texture ) == KTX_TRUE )
		{
		if ( ( Texture->classId != ktxTexture2_c ) || ( ktxTexture2_IsHDR ( ( ktxTexture2 * ) Texture ) == KTX_TRUE ) )
			{
			LOG_ERROR ( "Unable to transcode texture" );
			goto OnError;
			}
		Result = ktxTexture2_TranscodeBasis ( ( ktxTexture2 * ) Texture, KTX_TTF_RGBA32, 0 );
		if ( Result != KTX_SUCCESS )
			{
			LOG_ERROR ( "Unable to transcode texture - %s", ktxErrorString ( Result ) );
			goto OnError;
			}
		}

	crPixelFormat Format = crPixelFormat_Invalid;
	bool SwapRedBlue = false;
	if ( TranslateLoadedKTXFormat ( Texture, &Format, &SwapRedBlue ) == false )
		{
		LOG_ERROR ( "Unsupported texture format" );
		goto OnError;
		}

	struct crPixelFormatInfo PixelFormatInfo = crGetPixelFormatInfo ( Format );
	const unsigned DataSize = crPixelFormatDataSize ( Format, Texture->baseWidth, Texture->baseHeight );
	ktx_size_t Offset = 0;
	Result = ktxTexture_GetImageOffset ( Texture, 0, 0, 0, &Offset );
	if ( ( Result != KTX_SUCCESS ) || ( ktxTexture_GetData ( Texture ) == NULL ) || ( DataSize == 0 ) )
		{
		LOG_ERROR ( "Unable to read image data" );
		goto OnError;
		}
	const ktx_uint8_t *Image = ktxTexture_GetData ( Texture ) + Offset;

	Pixels = malloc ( DataSize );
	if ( Pixels == NULL )
		goto OnError;

	// Is texture data compressed?
	if ( PixelFormatInfo.BlockBytes != 0 )
		{
		if ( ktxTexture_GetImageSize ( Texture, 0 ) != ( ktx_size_t ) DataSize )
			{
			LOG_ERROR ( "Compressed image size does not match the format" );
			goto OnError;
			}
		memcpy ( Pixels, Image, DataSize );
		}
	else
		{
		const unsigned ElementSize = ktxTexture_GetElementSize ( Texture );
		const unsigned RowPitch = ktxTexture_GetRowPitch ( Texture, 0 );
		const unsigned TightPitch = Texture->baseWidth * ElementSize;
		if ( ( ElementSize == 0 ) || ( ElementSize != ( unsigned ) PixelFormatInfo.Bytes ) || ( RowPitch < TightPitch ) )
			{
			LOG_ERROR ( "Unexpected row layout" );
			goto OnError;
			}
		CopyUncompressedKTXImage ( Pixels, Image, Texture->baseWidth, Texture->baseHeight, RowPitch, ElementSize, SwapRedBlue );
		}

	Descriptor->Type = crTextureType_Texture2D;
	Descriptor->Dimensions.x = Texture->baseWidth;
	Descriptor->Dimensions.y = Texture->baseHeight;
	Descriptor->Format = Format;
	Descriptor->Mipmapped = PixelFormatInfo.Mipmaps;
	Descriptor->Data = Pixels;
	Descriptor->FilterSettings = ( crTextureFilterSettings )
		{
		crTextureFilter_Linear, crTextureFilter_Linear
		};
	Descriptor->WrapSettings = ( crTextureWrapSettings )
		{
		crTextureWrapMode_Repeat, crTextureWrapMode_Repeat
		};

	ktxTexture_Destroy ( Texture );
	return true;

OnError:
	free ( Pixels );
	ktxTexture_Destroy ( Texture );
	return false;
	}

bool LoadTextureDescriptorFromFileWithKTX ( const char *ImageFile, crTextureDescriptor *Descriptor )
	{
	ktxTexture *Texture = NULL;
	KTX_error_code Result = ktxTexture_CreateFromNamedFile ( ImageFile, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &Texture );
	if ( Result != KTX_SUCCESS )
		{
		LOG_ERROR ( "Unable to load texture from '%s' - %s", ImageFile, ktxErrorString ( Result ) );
		return false;
		}
	bool LoadResult = LoadTextureFromKTXTexture ( Texture, Descriptor );
	Descriptor->TextureName = strdup ( ImageFile );
	return LoadResult;
	}

bool LoadTextureDescriptorFromMemoryWithKTX ( const void *Pointer, const unsigned Length, crTextureDescriptor *Descriptor )
	{
	ktxTexture *Texture = NULL;
	KTX_error_code Result = ktxTexture_CreateFromMemory ( Pointer, Length, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &Texture );
	if ( Result != KTX_SUCCESS )
		{
		LOG_ERROR ( "Unable to load texture from memory - %s", ktxErrorString ( Result ) );
		return false;
		}
	return LoadTextureFromKTXTexture ( Texture, Descriptor );
	}