#pragma once
#include "../ShaderBuffer.h"

crShaderBufferHandle crGL4CreateShaderBuffer ( const struct crShaderBufferDescriptor CreationParameters );
bool crGL4DeleteShaderBuffer ( const crShaderBufferHandle Handle );
bool crGL4ChangeShaderBufferContents ( const crShaderBufferHandle Handle, const size_t Offset, const void *Data, const size_t DataSize );
void *crGL4MapShaderBuffer ( const crShaderBufferHandle Handle, const enum crShaderBufferMapAccessType AccessType );
bool crGL4UnmapShaderBuffer ( const crShaderBufferHandle Handle );
