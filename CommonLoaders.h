#pragma once
#include "CrossRenderer.h"

crShaderHandle LoadShader ( const char *VertexFile, const char *GeometryFile, const char *FragmentFile );
bool LoadFileContents ( const char *Filename, char **Contents );
bool LoadTextureDescriptorFromFile ( const char *ImageFile, const bool Flip, crTextureDescriptor *Descriptor );
bool LoadTextureDescriptorFromMemory ( const void *Pointer, const unsigned Length, const bool Flip, crTextureDescriptor *Descriptor );
crTextureHandle LoadTexture ( const char *ImageFile, const bool Flip );
crTextureHandle LoadCubemapTexture ( const char *ImageFile[6], const bool Flip );
