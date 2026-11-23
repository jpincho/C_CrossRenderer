#pragma once
#include <stdbool.h>
#include "CrossRenderer.h"

BEGIN_C_DECLARATIONS
bool IsFileLoadableWithKTX ( const char *Filename );
bool LoadTextureDescriptorFromFileWithKTX ( const char *ImageFile, crTextureDescriptor *Descriptor );
bool LoadTextureDescriptorFromMemoryWithKTX ( const void *Pointer, const unsigned Length, crTextureDescriptor *Descriptor );
END_C_DECLARATIONS
