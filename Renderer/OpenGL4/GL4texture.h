#pragma once
#include "../texture_descriptor.h"

typedef void* texture_handle_t;
const texture_handle_t texture_handle_invalid = NULL;

texture_handle_t CreateTexture ( const texture_descriptor_t Descriptor );
bool DeleteTexture( const texture_handle_t Handle );
bool Load2DTextureData ( const texture_handle_t handle, const EPixelFormat source_pixel_format, const void *data );
bool Update2DTextureRegion ( const texture_handle_t handle, const ivec2 LowerLeft, const ivec2 RegionDimensions, const EPixelFormat SourcePixelFormat, const void *Data );
bool LoadCubeMapTextureData ( const texture_handle_t handle, const EPixelFormat SourcePixelFormat, void *Data[6] );
void GetTextureDimensions ( const texture_handle_t Handle, ivec2 *Dimensions );
EPixelFormat GetTextureFormat ( const texture_handle_t handle );

