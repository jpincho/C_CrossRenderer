#pragma once
#include <Platform/defines.h>
#include <stdint.h>
#include <stdbool.h>

BEGIN_C_DECLARATIONS
typedef void *crShaderBufferHandle;
#define crShaderBufferHandle_Invalid NULL

typedef enum
	{
	crShaderBufferComponentType_Float = 0,
	crShaderBufferComponentType_Unsigned8,
	crShaderBufferComponentType_Unsigned16,
	crShaderBufferComponentType_Unsigned32
	} crShaderBufferComponentType;

typedef enum
	{
	crShaderBufferAccessType_Static,
	crShaderBufferAccessType_Dynamic,
	crShaderBufferAccessType_Stream
	} crShaderBufferAccessType;

typedef enum
	{
	crShaderBufferMapAccessType_ReadOnly,
	crShaderBufferMapAccessType_WriteOnly,
	crShaderBufferMapAccessType_ReadWrite
	} crShaderBufferMapAccessType;

typedef enum
	{
	crShaderBufferType_Array,
	crShaderBufferType_Uniform
	} crShaderBufferType;

typedef struct
	{
	crShaderBufferHandle BufferHandle;
	size_t StartOffset;
	size_t Stride;
	crShaderBufferComponentType ComponentType;
	size_t ComponentsPerElement;
	bool NormalizeData;
	bool PerInstance;
	} crShaderBufferDataStream;

typedef struct
	{
	size_t DataSize, Capacity;
	void *Data;
	crShaderBufferAccessType AccessType;
	crShaderBufferType BufferType;
	} crShaderBufferDescriptor;

inline void crDestroyShaderBufferDescriptor ( crShaderBufferDescriptor *Descriptor )
	{
	SAFE_DEL_C ( Descriptor->Data );
	}

#define SAFE_DELETE_SHADER_BUFFER(SHADER_BUFFER) do{if((SHADER_BUFFER) != crShaderBufferHandle_Invalid) {crDeleteShaderBuffer((SHADER_BUFFER));(SHADER_BUFFER)=crShaderBufferHandle_Invalid;}}while(0)
END_C_DECLARATIONS
