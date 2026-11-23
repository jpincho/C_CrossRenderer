#include "CrossRendererConfig.h"
#include "CommonLoaders.h"
#include "STBImageLoader.h"

#if defined ( CROSS_RENDERER_USE_KHRONOS_KTX )
#include "KTXImageLoader.h"
#endif

#include "PlatformIncludes.h"
#include <Platform/Logger.h>

#if defined (PLATFORM_COMPILER_GNU)
#include <fnmatch.h>
#elif defined (PLATFORM_COMPILER_MSVC)
#include <shlwapi.h>
#endif

#include "../Shader.h"
#include "../Texture.h"

static bool LoadFileContents ( const char *Filename, char **Contents )
	{
	FILE *FileHandle = NULL;
#if defined (PLATFORM_COMPILER_MSVC)
	if ( fopen_s ( &FileHandle, Filename, "rt" ) != 0 )
		return false;
#elif defined (PLATFORM_COMPILER_GNU)
	FileHandle = fopen ( Filename, "rt" );
#else
#error "Unhandled compiler"
#endif
	if ( FileHandle == NULL )
		return false;

	fseek ( FileHandle, 0, SEEK_END );
	size_t FileSize = ftell ( FileHandle );
	fseek ( FileHandle, 0, SEEK_SET );

	*Contents = calloc ( FileSize + 1, sizeof ( char ) );
	if ( *Contents == NULL )
		{
		fclose ( FileHandle );
		return false;
		}
	fread ( *Contents, sizeof ( char ), FileSize, FileHandle );
	fclose ( FileHandle );
	return true;
	}

crShaderHandle crLoadShader ( const char *VertexFile, const char *GeometryFile, const char *FragmentFile )
	{
	crShaderCode ShaderCode;
	crShaderHandle ShaderHandle = crShaderHandle_Invalid;
	ShaderCode.FragmentShader = ShaderCode.GeometryShader = ShaderCode.VertexShader = NULL;

	if ( LoadFileContents ( VertexFile, &ShaderCode.VertexShader ) == false )
		{
		LOG_ERROR ( "Unable to load vertex shader from '%s'", VertexFile );
		goto OnError;
		}
	if ( LoadFileContents ( FragmentFile, &ShaderCode.FragmentShader ) == false )
		{
		LOG_ERROR ( "Unable to load fragment shader from '%s'", FragmentFile );
		goto OnError;
		}
	if ( ( GeometryFile != NULL ) && ( LoadFileContents ( GeometryFile, &ShaderCode.GeometryShader ) == false ) )
		{
		LOG_ERROR ( "Unable to load geometry shader from '%s'", GeometryFile );
		goto OnError;
		}

	ShaderHandle = crCreateShader ( ShaderCode );
	if ( ShaderHandle == crShaderHandle_Invalid )
		LOG_ERROR ( "Unable to build shader from '%s' and '%s'", VertexFile, FragmentFile );

OnError:
	SAFE_DEL_C ( ShaderCode.FragmentShader );
	SAFE_DEL_C ( ShaderCode.VertexShader );
	SAFE_DEL_C ( ShaderCode.GeometryShader );
	return ShaderHandle;
	}

bool crLoadTextureDescriptorFromFile ( const char *ImageFile, crTextureDescriptor *Descriptor )
	{
#if defined ( CROSS_RENDERER_USE_KHRONOS_KTX )
	if ( IsFileLoadableWithKTX ( ImageFile ) )
		return LoadTextureDescriptorFromFileWithKTX ( ImageFile, Descriptor );
#endif
	return LoadTextureDescriptorFromFileWithSTBI ( ImageFile, Descriptor );
	}

bool crLoadTextureDescriptorFromMemory ( const void *Pointer, const unsigned Length, crTextureDescriptor *Descriptor )
	{
#if defined ( CROSS_RENDERER_USE_KHRONOS_KTX )
	if ( LoadTextureDescriptorFromMemoryWithKTX ( Pointer, Length, Descriptor ) == true )
		return true;
#endif
	return LoadTextureDescriptorFromMemoryWithSTBI ( Pointer, Length, Descriptor );
	}

crTextureHandle crLoadTexture ( const char *ImageFile )
	{
	crTextureDescriptor TextureDescriptor;
	if ( crLoadTextureDescriptorFromFile ( ImageFile, &TextureDescriptor ) == false )
		return crTextureHandle_Invalid;

	crTextureHandle Texture = crCreateTexture ( TextureDescriptor );
	free ( TextureDescriptor.Data );
	return Texture;
	}

crTextureHandle crLoadCubemapTexture ( const char *ImageFile[6] )
	{
	crTextureDescriptor FaceDescriptor[6] = {0};
	for ( unsigned Face = 0; Face < 6; ++Face )
		{
		if ( crLoadTextureDescriptorFromFile ( ImageFile[Face], &FaceDescriptor[Face] ) == false )
			goto OnError;
		}

	crTextureDescriptor TextureDescriptor = { 0 };
	TextureDescriptor.Type = crTextureType_TextureCubeMap;
	TextureDescriptor.Dimensions = FaceDescriptor[0].Dimensions;
	TextureDescriptor.Format = FaceDescriptor[0].Format;
	TextureDescriptor.Mipmapped = true;
	TextureDescriptor.Data = NULL;
	crTextureHandle Texture = crCreateTexture ( TextureDescriptor );

	void *VoidPointer[6];
	for ( unsigned Face = 0; Face < 6; ++Face )
		VoidPointer[Face] = FaceDescriptor[Face].Data;
	if ( crLoadCubeMapTextureData ( Texture, FaceDescriptor[0].Format, VoidPointer ) == false )
		goto OnError;

	for ( unsigned Face = 0; Face < 6; ++Face )
		crDestroyTextureDescriptor ( &FaceDescriptor[Face] );
	return Texture;

OnError:
	for ( unsigned Face = 0; Face < 6; ++Face )
		crDestroyTextureDescriptor ( &FaceDescriptor[Face] );
	return crTextureHandle_Invalid;
	}

bool crIsValidTextureFile ( const char *Filename )
	{
#if defined ( CROSS_RENDERER_USE_KHRONOS_KTX )
	if ( IsFileLoadableWithKTX ( Filename ) )
		return true;
#endif
	if ( IsFileLoadableWithSTBI ( Filename ) )
		return true;
	return false;
	}
