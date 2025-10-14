#pragma once
#include "../Texture.h"

crTextureHandle crGL4CreateTexture ( const struct crTextureDescriptor Descriptor );
bool crGL4DeleteTexture( const crTextureHandle Handle );
bool crGL4Load2DTextureData ( const crTextureHandle Handle, const enum crPixelFormat SourcePixelFormat, const void *data );
bool crGL4Update2DTextureRegion ( const crTextureHandle Handle, const uvec2 LowerLeft, const uvec2 RegionDimensions, const enum crPixelFormat SourcePixelFormat, const void *Data );
bool crGL4LoadCubeMapTextureData ( const crTextureHandle Handle, const enum crPixelFormat SourcePixelFormat, void *Data[6] );
void crGL4GetTextureDimensions ( const crTextureHandle Handle, uvec2 *Dimensions );
enum crPixelFormat crGL4GetTextureFormat ( const crTextureHandle Handle );

