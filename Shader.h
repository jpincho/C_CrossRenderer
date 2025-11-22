#pragma once
#include <Math/mathlib.h>
#include <Platform/defines.h>
#include "ShaderBuffer.h"
#include <stdint.h>
#include <stdbool.h>

typedef void *crShaderHandle;
#define crShaderHandle_Invalid NULL

typedef int crShaderAttributeHandle;
typedef int crShaderUniformHandle;

typedef struct
	{
	char *VertexShader;
	char *FragmentShader;
	char *GeometryShader;
	} crShaderCode;

typedef enum
	{
	crShaderObjectType_Vertex,
	crShaderObjectType_Geometry,
	crShaderObjectType_Fragment
	} crShaderObjectType;

typedef enum
	{
	crShaderUniformType_Bool = 0,
	crShaderUniformType_Bool2,
	crShaderUniformType_Bool3,
	crShaderUniformType_Bool4,
	crShaderUniformType_Float,
	crShaderUniformType_Float2,
	crShaderUniformType_Float3,
	crShaderUniformType_Float4,
	crShaderUniformType_UnsignedInteger,
	crShaderUniformType_UnsignedInteger2,
	crShaderUniformType_UnsignedInteger3,
	crShaderUniformType_UnsignedInteger4,
	crShaderUniformType_Integer,
	crShaderUniformType_Integer2,
	crShaderUniformType_Integer3,
	crShaderUniformType_Integer4,
	crShaderUniformType_Matrix2,
	crShaderUniformType_Matrix3,
	crShaderUniformType_Matrix4,

	crShaderUniformType_Sampler2D,
	crShaderUniformType_Sampler3D,
	crShaderUniformType_SamplerCube,
	crShaderUniformType_Block,
	crShaderUniformType_Unknown
	} crShaderUniformType;

typedef enum
	{
	crShaderAttributeType_Bool = 0,
	crShaderAttributeType_Bool2,
	crShaderAttributeType_Bool3,
	crShaderAttributeType_Bool4,
	crShaderAttributeType_Float,
	crShaderAttributeType_Float2,
	crShaderAttributeType_Float3,
	crShaderAttributeType_Float4,
	crShaderAttributeType_UnsignedInteger,
	crShaderAttributeType_UnsignedInteger2,
	crShaderAttributeType_UnsignedInteger3,
	crShaderAttributeType_UnsignedInteger4,
	crShaderAttributeType_Integer,
	crShaderAttributeType_Integer2,
	crShaderAttributeType_Integer3,
	crShaderAttributeType_Integer4,
	crShaderAttributeType_Matrix2,
	crShaderAttributeType_Matrix3,
	crShaderAttributeType_Matrix4,

	crShaderAttributeType_Sampler2D,
	crShaderAttributeType_Sampler3D,
	crShaderAttributeType_SamplerCube,
	crShaderAttributeType_Block,
	crShaderAttributeType_Unknown
	} crShaderAttributeType;

typedef struct
	{
	const char *Name;
	int Handle;
	crShaderUniformType Type;
	} crShaderUniformInformation;

typedef struct
	{
	const char *Name;
	int Handle;
	crShaderAttributeType Type;
	} crShaderAttributeInformation;

typedef struct
	{
	crShaderUniformInformation *Uniforms, *UniformBlocks;
	unsigned UniformCount, UniformBlockCount;
	crShaderAttributeInformation *Attributes;
	unsigned AttributeCount;
	crShaderHandle Handle;
	} crShaderInformation;

typedef struct
	{
	union
		{
		bool BoolValue;
		ivec2 Bool2Value;
		ivec3 Bool3Value;
		ivec4 Bool4Value;

		float FloatValue;
		vec2 Float2Value;
		vec3 Float3Value;
		vec4 Float4Value;

		uint32_t UnsignedIntegerValue;
		uvec2 UnsignedInteger2Value;
		uvec3 UnsignedInteger3Value;
		uvec4 UnsignedInteger4Value;

		int32_t IntegerValue;
		ivec2 Integer2Value;
		ivec3 Integer3Value;
		ivec4 Integer4Value;

		mat2 Matrix2Value;
		mat3 Matrix3Value;
		mat4 Matrix4Value;

		crShaderBufferHandle BlockValue;
		};
	} crShaderUniformValue;

inline void crDestroyShaderInformation ( crShaderInformation *Information )
	{
	SAFE_DEL_C ( Information->Attributes );
	SAFE_DEL_C ( Information->Uniforms );
	SAFE_DEL_C ( Information->UniformBlocks );
	}

BEGIN_C_DECLARATIONS
crShaderUniformHandle crGetShaderUniformHandle ( const crShaderHandle ShaderHandle, const char *UniformName );
crShaderAttributeHandle crGetShaderAttributeHandle ( const crShaderHandle ShaderHandle, const char *AttributeName );
END_C_DECLARATIONS
