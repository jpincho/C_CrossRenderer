#pragma once
#include "../ShaderBuffer.h"

BEGIN_C_DECLARATIONS
crShaderBufferHandle crGL4CreateShaderBuffer ( const crShaderBufferDescriptor CreationParameters );
bool crGL4DeleteShaderBuffer ( const crShaderBufferHandle Handle );
bool crGL4ChangeShaderBufferContents ( const crShaderBufferHandle Handle, const size_t Offset, const void *Data, const size_t DataSize );
void *crGL4MapShaderBuffer ( const crShaderBufferHandle Handle, const crShaderBufferMapAccessType AccessType );
bool crGL4UnmapShaderBuffer ( const crShaderBufferHandle Handle );
END_C_DECLARATIONS
