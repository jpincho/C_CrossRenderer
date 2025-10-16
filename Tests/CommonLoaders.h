#pragma once
#include "../CrossRenderer.h"

crShaderHandle LoadShader ( const char *VertexFile, const char *GeometryFile, const char *FragmentFile );
bool LoadFileContents ( const char *Filename, char **Contents );
crTextureHandle LoadTexture ( const char *ImageFile, const bool Flip );
crTextureHandle LoadCubemapTexture ( const char *ImageFile[6], const bool Flip );
