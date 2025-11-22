#include "GL4Texture.h"
#include "GL4Internals.h"
#include <assert.h>

// Texture creation
// OriginPixelFormat - GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_RED_INTEGER, GL_RG_INTEGER, GL_RGB_INTEGER, GL_BGR_INTEGER, GL_RGBA_INTEGER, GL_BGRA_INTEGER, GL_STENCIL_INDEX, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL.
// DataType - GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_HALF_FLOAT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV.
// DestinationInternalFormat - GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL, GL_RED, GL_RG, GL_RGB, GL_RGBA
// (Sized) - GL_R8, GL_R8_SNORM, GL_R16, GL_R16_SNORM, GL_RG8, GL_RG8_SNORM, GL_RG16, GL_RG16_SNORM, GL_R3_G3_B2, GL_RGB4, GL_RGB5, GL_RGB8, GL_RGB8_SNORM, GL_RGB10, GL_RGB12, GL_RGB16_SNORM, GL_RGBA2, GL_RGBA4, GL_RGB5_A1
// GL_RGBA8, GL_RGBA8_SNORM, GL_RGB10_A2, GL_RGB10_A2UI, GL_RGBA12, GL_RGBA16, GL_SRGB8, GL_SRGB8_ALPHA8, GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F, GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F, GL_R11F_G11F_B10F, GL_RGB9_E5
// GL_R8I, GL_R8UI, GL_R16I, GL_R16UI, GL_R32I, GL_R32UI, GL_RG8I, GL_RG8UI, GL_RG16I, GL_RG16UI, GL_RG32I, GL_RG32UI, GL_RGB8I, GL_RGB8UI, GL_RGB16I, GL_RGB16UI, GL_RGB32I, GL_RGB32UI, GL_RGBA8I, GL_RGBA8UI, GL_RGBA16I, GL_RGBA16UI, GL_RGBA32I, GL_RGBA32UI
// (Compressed) - GL_COMPRESSED_RED, GL_COMPRESSED_RG, GL_COMPRESSED_RGB, GL_COMPRESSED_RGBA, GL_COMPRESSED_SRGB, GL_COMPRESSED_SRGB_ALPHA, GL_COMPRESSED_RED_RGTC1, GL_COMPRESSED_SIGNED_RED_RGTC1, GL_COMPRESSED_RG_RGTC2
// GL_COMPRESSED_SIGNED_RG_RGTC2, GL_COMPRESSED_RGBA_BPTC_UNORM, GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT, GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT

// Texture update
// OriginPixelFormat - GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_DEPTH_COMPONENT, and GL_STENCIL_INDEX.
// DataType - GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV.

struct StructPixelFormatData
	{
	GLint DataType;
	GLint Format;
	GLint SizedFormat;
	};

static struct StructPixelFormatData PixelFormatData[] =
	{
		{GL_UNSIGNED_BYTE, GL_RED, GL_R8},// Red8
		{GL_UNSIGNED_BYTE, GL_RG, GL_RG8},// RedGreen88
		{GL_UNSIGNED_BYTE, GL_RGB, GL_RGB8},// RedGreenBlue888
		{GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA8},// RedGreenBlueAlpha8888
		{GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA8},// AlphaRedGreenBlue8888
		{GL_UNSIGNED_SHORT, GL_RGB, GL_RGB565},// RedGreenBlue565
		{GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24}// DepthComponent
	};

crTextureHandle crGL4CreateTexture(const crTextureDescriptor Descriptor)
	{
	crGL4InternalTextureInfo *NewTexture = calloc(1, sizeof(crGL4InternalTextureInfo));
	assert(NewTexture);
	pointer_list_node *NewNode = NULL;

	NewTexture->GLSWrap = crGL4TranslateTextureWrapMode(Descriptor.WrapSettings.Horizontal);
	NewTexture->GLTWrap = crGL4TranslateTextureWrapMode(Descriptor.WrapSettings.Vertical);
	NewTexture->GLMinFilter = crGL4TranslateTextureFilter(Descriptor.FilterSettings.MinFilter);
	NewTexture->GLMagFilter = crGL4TranslateTextureFilter(Descriptor.FilterSettings.MagFilter);
	NewTexture->GLTextureType = crGL4TranslateTextureType(Descriptor.Type);
	math_uvec2_copy(&NewTexture->Dimensions, Descriptor.Dimensions);
	NewTexture->Mipmapped = Descriptor.Mipmapped;
	NewTexture->Format = Descriptor.Format;

	if (crGL4Information.DirectStateAccessEnabled)
		{
		glCreateTextures(NewTexture->GLTextureType, 1, &NewTexture->OpenGLID);
		if (crGL4CheckError() == false)
			goto OnError;

		//glTextureParameteri(NewTexture->OpenGLID, GL_TEXTURE_WRAP_S, NewTexture->GLSWrap);
		//glTextureParameteri(NewTexture->OpenGLID, GL_TEXTURE_WRAP_T, NewTexture->GLTWrap);
		//glTextureParameteri(NewTexture->OpenGLID, GL_TEXTURE_MIN_FILTER, NewTexture->GLMinFilter);
		//glTextureParameteri(NewTexture->OpenGLID, GL_TEXTURE_MAG_FILTER, NewTexture->GLMagFilter);
		if (NewTexture->GLTextureType == GL_TEXTURE_CUBE_MAP)
			glTextureParameteri(NewTexture->OpenGLID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTextureStorage2D(NewTexture->OpenGLID,
		                   1,
		                   PixelFormatData[Descriptor.Format].SizedFormat,
		                   NewTexture->Dimensions.x, NewTexture->Dimensions.y);
		}
	else
		{
		glGenTextures(1, &NewTexture->OpenGLID);
		if (crGL4CheckError() == false)
			goto OnError;

		glBindTexture(NewTexture->GLTextureType, NewTexture->OpenGLID);
		//glTexParameteri(NewTexture->GLTextureType, GL_TEXTURE_WRAP_S, NewTexture->GLSWrap);
		//glTexParameteri(NewTexture->GLTextureType, GL_TEXTURE_WRAP_T, NewTexture->GLTWrap);
		//glTexParameteri(NewTexture->GLTextureType, GL_TEXTURE_MIN_FILTER, NewTexture->GLMinFilter);
		//glTexParameteri(NewTexture->GLTextureType, GL_TEXTURE_MAG_FILTER, NewTexture->GLMagFilter);
		if (NewTexture->GLTextureType == GL_TEXTURE_CUBE_MAP)
			glTexParameteri(NewTexture->GLTextureType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexStorage2D(NewTexture->GLTextureType,
		               1,
		               PixelFormatData[Descriptor.Format].SizedFormat,
		               NewTexture->Dimensions.x, NewTexture->Dimensions.y);
		glBindTexture(GL_TEXTURE_2D, 0);
		}

	if (crGL4CheckError() == false)
		goto OnError;

	NewNode = PointerList_AddAtEnd(&crGL4Information.Textures, NewTexture);
	if (Descriptor.Data != NULL)
		{
		if (crGL4Load2DTextureData(NewNode, Descriptor.Format, Descriptor.Data) == false)
			{
			glDeleteTextures(1, &NewTexture->OpenGLID);
			goto OnError;
			}
		}
	return NewNode;

OnError:
	SAFE_DEL_C(NewTexture);
	if (NewNode)
		PointerList_DestroyNode(&crGL4Information.Textures, NewNode);
	return crTextureHandle_Invalid;
	}

bool crGL4DeleteTexture(const crTextureHandle Handle)
	{
	crGL4InternalTextureInfo *TextureInformation = (crGL4InternalTextureInfo *)PointerList_GetNodeData(Handle);

	glDeleteTextures(1, &TextureInformation->OpenGLID);
	free(TextureInformation);
	PointerList_DestroyNode(&crGL4Information.Textures, Handle);
	return true;
	}

bool crGL4Load2DTextureData(const crTextureHandle Handle, const crPixelFormat SourcePixelFormat, const void *Data)
	{
	crGL4InternalTextureInfo *TextureInformation = (crGL4InternalTextureInfo *)PointerList_GetNodeData(Handle);

	return crGL4Update2DTextureRegion(Handle, (uvec2)
		{
		0
		}, TextureInformation->Dimensions, SourcePixelFormat, Data);
	}

bool crGL4Update2DTextureRegion(const crTextureHandle Handle, const uvec2 LowerLeft, const uvec2 RegionDimensions, const crPixelFormat SourcePixelFormat, const void *Data)
	{
	crGL4InternalTextureInfo *TextureInformation = (crGL4InternalTextureInfo *)PointerList_GetNodeData(Handle);

	if ((LowerLeft.x + RegionDimensions.x > TextureInformation->Dimensions.x) ||
	        (LowerLeft.y + RegionDimensions.y > TextureInformation->Dimensions.y))
		return false;

	// TODO Rewrite this pixel storage part
	uint8_t alignment = 1;
	uint32_t alignment_mask;
	for (alignment_mask = 1; alignment_mask <= 8; alignment_mask *= 2)
		{
		if ((RegionDimensions.x % alignment_mask == 0) && ((intptr_t)(Data) % alignment_mask == 0))
			alignment = alignment_mask & 0xFF;
		}

	// Set pixel storage parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, alignment); // Ensure no padding between rows
	//glPixelStorei(GL_UNPACK_ROW_LENGTH, 0); // Default row length
	//glPixelStorei(GL_UNPACK_SKIP_ROWS, 0); // No rows skipped
	//glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0); // No pixels skipped

	if (crGL4Information.DirectStateAccessEnabled)
		{
		glTextureSubImage2D(TextureInformation->OpenGLID,
		                    0,
		                    LowerLeft.x, LowerLeft.y,
		                    (int)RegionDimensions.x, (int)RegionDimensions.y,
		                    PixelFormatData[(int)SourcePixelFormat].Format,
		                    PixelFormatData[(int)SourcePixelFormat].DataType,
		                    Data);
		if (!crGL4CheckError())
			return false;
		if (TextureInformation->Mipmapped)
			glGenerateTextureMipmap(TextureInformation->OpenGLID);
		}
	else
		{
		glBindTexture(GL_TEXTURE_2D, TextureInformation->OpenGLID);
		glTexSubImage2D(GL_TEXTURE_2D,
		                0,
		                LowerLeft.x, LowerLeft.y,
		                (int)RegionDimensions.x, (int)RegionDimensions.y,
		                PixelFormatData[(int)SourcePixelFormat].Format,
		                PixelFormatData[(int)SourcePixelFormat].DataType,
		                Data);
		if (!crGL4CheckError())
			return false;
		if (TextureInformation->Mipmapped)
			glGenerateMipmap(GL_TEXTURE_2D);
		}

	return true;
	}

bool crGL4LoadCubeMapTextureData(const crTextureHandle Handle, const crPixelFormat SourcePixelFormat, void *Data[6])
	{
	crGL4InternalTextureInfo *TextureInformation = (crGL4InternalTextureInfo *)PointerList_GetNodeData(Handle);

	if (crGL4Information.DirectStateAccessEnabled)
		{
		for (unsigned Face = 0; Face < 6; ++Face)
			{
			glTextureSubImage3D(TextureInformation->OpenGLID,
			                    0,
			                    0, 0, Face,
			                    TextureInformation->Dimensions.x, TextureInformation->Dimensions.y, 1,
			                    PixelFormatData[(int)SourcePixelFormat].Format,
			                    PixelFormatData[(int)SourcePixelFormat].DataType,
			                    Data[Face]);
			}
		if (!crGL4CheckError())
			return false;
		if (TextureInformation->Mipmapped)
			glGenerateTextureMipmap(TextureInformation->OpenGLID);
		}
	else
		{
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureInformation->OpenGLID);
		for (unsigned Face = 0; Face < 6; ++Face)
			{
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face,
			                0,
			                0, 0,
			                TextureInformation->Dimensions.x, TextureInformation->Dimensions.y,
			                PixelFormatData[(int)SourcePixelFormat].Format,
			                PixelFormatData[(int)SourcePixelFormat].DataType,
			                Data[Face]);
			}
		if (TextureInformation->Mipmapped)
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}
	return crGL4CheckError();
	}

void crGL4GetTextureDimensions(const crTextureHandle Handle, uvec2 *Dimensions)
	{
	crGL4InternalTextureInfo *TextureInformation = (crGL4InternalTextureInfo *)PointerList_GetNodeData(Handle);

	math_uvec2_copy(Dimensions, TextureInformation->Dimensions);
	}

crPixelFormat crGL4GetTextureFormat(const crTextureHandle Handle)
	{
	crGL4InternalTextureInfo *TextureInformation = (crGL4InternalTextureInfo *)PointerList_GetNodeData(Handle);

	return TextureInformation->Format;
	}
