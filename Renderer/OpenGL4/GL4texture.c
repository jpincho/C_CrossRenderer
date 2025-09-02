#include "GL4texture.h"
#include "GL4Internals.h"
#include "../../../C_Common/pointer_list.h"

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

typedef struct
	{
	GLint DataType;
	GLint Format;
	GLint SizedFormat;
	} StructPixelFormatData;

static pointer_list GL4Textures = { 0 };

static StructPixelFormatData PixelFormatData[] =
	{
		{GL_UNSIGNED_BYTE, GL_RED, GL_R8},// Red8
		{GL_UNSIGNED_BYTE, GL_RG, GL_RG8},// RedGreen88
		{GL_UNSIGNED_BYTE, GL_RGB, GL_RGB8},// RedGreenBlue888
		{GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA8},// RedGreenBlueAlpha8888
		{GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA8},// AlphaRedGreenBlue8888
		{GL_UNSIGNED_SHORT, GL_RGB, GL_RGB565},// RedGreenBlue565
		{GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24}// DepthComponent
	};

texture_handle_t CreateTexture(const texture_descriptor_t Descriptor)
	{
	GLInternalTextureInfo *NewTexture = calloc(1, sizeof(GLInternalTextureInfo));
	pointer_list_node *NewNode = NULL;

	TranslateTextureWrapMode(Descriptor.WrapSettings.horizontal, &NewTexture->GLSWrap);
	TranslateTextureWrapMode(Descriptor.WrapSettings.vertical, &NewTexture->GLTWrap);
	TranslateTextureFilter(Descriptor.FilterSettings.min_filter, &NewTexture->GLMinFilter);
	TranslateTextureFilter(Descriptor.FilterSettings.mag_filter, &NewTexture->GLMagFilter);
	TranslateTextureType(Descriptor.Type, &NewTexture->GLTextureType);
	glm_ivec2_copy((int*)Descriptor.Dimensions, NewTexture->Dimensions);
	NewTexture->Mipmapped = Descriptor.mipmapped;
	NewTexture->Format = Descriptor.Format;

	if (OpenGLInformation.DirectStateAccessEnabled)
		{
		glCreateTextures(NewTexture->GLTextureType, 1, &NewTexture->OpenGLID);
		if (CheckError() == false)
			goto OnError;

		glTextureParameteri(NewTexture->OpenGLID, GL_TEXTURE_WRAP_S, NewTexture->GLSWrap);
		glTextureParameteri(NewTexture->OpenGLID, GL_TEXTURE_WRAP_T, NewTexture->GLTWrap);
		glTextureParameteri(NewTexture->OpenGLID, GL_TEXTURE_MIN_FILTER, NewTexture->GLMinFilter);
		glTextureParameteri(NewTexture->OpenGLID, GL_TEXTURE_MAG_FILTER, NewTexture->GLMagFilter);
		if (NewTexture->GLTextureType == GL_TEXTURE_CUBE_MAP)
			glTextureParameteri(NewTexture->OpenGLID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTextureStorage2D(NewTexture->OpenGLID,
			1,
			PixelFormatData[Descriptor.Format].SizedFormat,
			NewTexture->Dimensions[0], NewTexture->Dimensions[1]);
		}
	else
		{
		glGenTextures(1, &NewTexture->OpenGLID);
		if (CheckError() == false)
			goto OnError;

		glBindTexture(NewTexture->GLTextureType, NewTexture->OpenGLID);
		glTexParameteri(NewTexture->GLTextureType, GL_TEXTURE_WRAP_S, NewTexture->GLSWrap);
		glTexParameteri(NewTexture->GLTextureType, GL_TEXTURE_WRAP_T, NewTexture->GLTWrap);
		glTexParameteri(NewTexture->GLTextureType, GL_TEXTURE_MIN_FILTER, NewTexture->GLMinFilter);
		glTexParameteri(NewTexture->GLTextureType, GL_TEXTURE_MAG_FILTER, NewTexture->GLMagFilter);
		if (NewTexture->GLTextureType == GL_TEXTURE_CUBE_MAP)
			glTexParameteri(NewTexture->GLTextureType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexStorage2D(NewTexture->GLTextureType,
			1,
			PixelFormatData[Descriptor.Format].SizedFormat,
			NewTexture->Dimensions[0], NewTexture->Dimensions[1]);
		glBindTexture(GL_TEXTURE_2D, 0);
		}

	if (CheckError() == false)
		goto OnError;


	NewNode = pointer_list_add_at_end(&GL4Textures, NewTexture);
	if (Descriptor.Data != NULL)
		{
		if (Load2DTextureData(NewNode, Descriptor.Format, Descriptor.Data) == false)
			{
			glDeleteTextures(1, &NewTexture->OpenGLID);
			goto OnError;
			}
		}
	return NewNode;

OnError:
	SAFE_DEL_C(NewTexture);
	if (NewNode)
		pointer_list_destroy_node(&GL4Textures, NewNode);
	return texture_handle_invalid;
	}

bool DeleteTexture(const texture_handle_t Handle)
	{
	GLInternalTextureInfo *TextureInformation = (GLInternalTextureInfo * )pointer_list_get_node_data(Handle);

	glDeleteTextures(1, &TextureInformation->OpenGLID);
	free(TextureInformation);
	pointer_list_destroy_node(&GL4Textures, Handle);
	return true;
	}

bool Load2DTextureData(const texture_handle_t Handle, const EPixelFormat SourcePixelFormat, const void *Data)
	{
	GLInternalTextureInfo *TextureInformation = (GLInternalTextureInfo *)pointer_list_get_node_data(Handle);

	return Update2DTextureRegion(Handle, GLM_IVEC2_ZERO, TextureInformation->Dimensions, SourcePixelFormat, Data);
	}

bool Update2DTextureRegion(const texture_handle_t Handle, const ivec2 LowerLeft, const ivec2 RegionDimensions, const EPixelFormat SourcePixelFormat, const void *Data)
	{
	GLInternalTextureInfo *TextureInformation = (GLInternalTextureInfo *)pointer_list_get_node_data(Handle);

	if ((LowerLeft[0] + RegionDimensions[0] > TextureInformation->Dimensions[0]) ||
		(LowerLeft[1] + RegionDimensions[1] > TextureInformation->Dimensions[1]))
		return false;

	// TODO Rewrite this pixel storage part
	uint8_t alignment = 1;
	uint32_t alignment_mask;
	for (alignment_mask = 1; alignment_mask <= 8; alignment_mask *= 2)
		{
		if ((RegionDimensions[0] % alignment_mask == 0) && ((intptr_t)(Data) % alignment_mask == 0))
			alignment = alignment_mask & 0xFF;
		}

	// Set pixel storage parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, alignment); // Ensure no padding between rows
	//glPixelStorei(GL_UNPACK_ROW_LENGTH, 0); // Default row length
	//glPixelStorei(GL_UNPACK_SKIP_ROWS, 0); // No rows skipped
	//glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0); // No pixels skipped

	if (OpenGLInformation.DirectStateAccessEnabled)
		{
		glTextureSubImage2D(TextureInformation->OpenGLID,
			0,
			LowerLeft[0], LowerLeft[1],
			(int)RegionDimensions[0], (int)RegionDimensions[1],
			PixelFormatData[(int)SourcePixelFormat].Format,
			PixelFormatData[(int)SourcePixelFormat].DataType,
			Data);
		if (!CheckError())
			return false;
		if (TextureInformation->Mipmapped)
			glGenerateTextureMipmap(TextureInformation->OpenGLID);
		}
	else
		{
		glBindTexture(GL_TEXTURE_2D, TextureInformation->OpenGLID);
		glTexSubImage2D(GL_TEXTURE_2D,
			0,
			LowerLeft[0], LowerLeft[1],
			(int)RegionDimensions[0], (int)RegionDimensions[1],
			PixelFormatData[(int)SourcePixelFormat].Format,
			PixelFormatData[(int)SourcePixelFormat].DataType,
			Data);
		if (!CheckError())
			return false;
		if (TextureInformation->Mipmapped)
			glGenerateMipmap(GL_TEXTURE_2D);
		}

	return true;
	}

bool LoadCubeMapTextureData(const texture_handle_t Handle, const EPixelFormat SourcePixelFormat, void *Data[6])
	{
	GLInternalTextureInfo *TextureInformation = (GLInternalTextureInfo *)pointer_list_get_node_data(Handle);

	if (OpenGLInformation.DirectStateAccessEnabled)
		{
		for (unsigned Face = 0; Face < 6; ++Face)
			{
			glTextureSubImage3D(TextureInformation->OpenGLID,
				0,
				0, 0, Face,
				TextureInformation->Dimensions[0], TextureInformation->Dimensions[1], 1,
				PixelFormatData[(int)SourcePixelFormat].Format,
				PixelFormatData[(int)SourcePixelFormat].DataType,
				Data[Face]);
			}
		if (!CheckError())
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
				TextureInformation->Dimensions[0], TextureInformation->Dimensions[1],
				PixelFormatData[(int)SourcePixelFormat].Format,
				PixelFormatData[(int)SourcePixelFormat].DataType,
				Data[Face]);
			}
		if (TextureInformation->Mipmapped)
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}
	return CheckError();
	}

void GetTextureDimensions(const texture_handle_t Handle, ivec2 *Dimensions)
	{
	GLInternalTextureInfo *TextureInformation = (GLInternalTextureInfo *)pointer_list_get_node_data(Handle);

	glm_ivec2_copy(TextureInformation->Dimensions, *Dimensions);
	}

EPixelFormat GetTextureFormat(const texture_handle_t Handle)
	{
	GLInternalTextureInfo *TextureInformation = (GLInternalTextureInfo *)pointer_list_get_node_data(Handle);

	return TextureInformation->Format;
	}
