#pragma once
#include "CrossRenderer.h"

BEGIN_C_DECLARATIONS
crShaderHandle crLoadShader ( const char *VertexFile, const char *GeometryFile, const char *FragmentFile );
bool crLoadTextureDescriptorFromFile ( const char *ImageFile, const bool Flip, crTextureDescriptor *Descriptor );
bool crLoadTextureDescriptorFromMemory ( const void *Pointer, const unsigned Length, const bool Flip, crTextureDescriptor *Descriptor );
crTextureHandle crLoadTexture ( const char *ImageFile, const bool Flip );
crTextureHandle crLoadCubemapTexture ( const char *ImageFile[6], const bool Flip );
bool crIsValidTextureFile ( const char *Filename );
END_C_DECLARATIONS
