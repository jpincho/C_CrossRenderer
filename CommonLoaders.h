#pragma once
#include "CrossRenderer.h"

BEGIN_C_DECLARATIONS
crShaderHandle crLoadShader ( const char *VertexFile, const char *GeometryFile, const char *FragmentFile );
bool crLoadTextureDescriptorFromFile ( const char *ImageFile, crTextureDescriptor *Descriptor );
bool crLoadTextureDescriptorFromMemory ( const void *Pointer, const unsigned Length, crTextureDescriptor *Descriptor );
crTextureHandle crLoadTexture ( const char *ImageFile );
crTextureHandle crLoadCubemapTexture ( const char *ImageFile[6] );
bool crIsValidTextureFile ( const char *Filename );
END_C_DECLARATIONS
