#pragma once
#include <Platform/defines.h>
#include <stdint.h>
#include <stdbool.h>

typedef void *crShaderBufferHandle;
#define ShaderBufferHandle_Invalid NULL

enum crShaderBufferComponentType
	{
	crShaderBufferComponentType_Float = 0,
	crShaderBufferComponentType_Unsigned8,
	crShaderBufferComponentType_Unsigned16,
	crShaderBufferComponentType_Unsigned32
	};

enum crShaderBufferAccessType
	{
	crShaderBufferAccessType_Static,
	crShaderBufferAccessType_Dynamic,
	crShaderBufferAccessType_Stream
	};

enum crShaderBufferMapAccessType
	{
	crShaderBufferMapAccessType_ReadOnly,
	crShaderBufferMapAccessType_WriteOnly,
	crShaderBufferMapAccessType_ReadWrite
	};

enum crShaderBufferType
	{
	crShaderBufferType_Array,
	crShaderBufferType_Uniform
	};

struct crShaderBufferDataStream
	{
	crShaderBufferHandle BufferHandle;
	size_t StartOffset;
	size_t Stride;
	enum crShaderBufferComponentType ComponentType;
	size_t ComponentsPerElement;
	bool NormalizeData;
	bool PerInstance;
	};

struct crShaderBufferDescriptor
	{
	size_t DataSize, Capacity;
	void *Data;
	enum crShaderBufferAccessType AccessType;
	enum crShaderBufferType BufferType;
	};

inline void crDestroyShaderBufferDescriptor ( struct crShaderBufferDescriptor *Descriptor )
	{
	SAFE_DEL_C ( Descriptor->Data );
	}
