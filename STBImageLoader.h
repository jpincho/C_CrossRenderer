#pragma once
#include <stdbool.h>
#include "CrossRenderer.h"

BEGIN_C_DECLARATIONS
bool IsFileLoadableWithSTBI ( const char *Filename );
bool LoadTextureDescriptorFromFileWithSTBI ( const char *ImageFile, crTextureDescriptor *Descriptor );
bool LoadTextureDescriptorFromMemoryWithSTBI ( const void *Pointer, const unsigned Length, crTextureDescriptor *Descriptor );
END_C_DECLARATIONS
