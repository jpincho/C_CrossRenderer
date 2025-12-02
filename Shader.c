#include "Shader.h"
#include "Internal/CrossRendererFunctionPointers.h"
#include <string.h>

crShaderUniformHandle crGetShaderUniformHandle(const crShaderHandle ShaderHandle, const char *UniformName)
	{
	const crShaderInformation *ShaderInformation = crGetShaderInformation(ShaderHandle);
	if (ShaderInformation == NULL)
		return -1;
	for (unsigned Index = 0; Index < ShaderInformation->UniformCount; ++Index)
		{
		if (strcmp(ShaderInformation->Uniforms[Index].Name, UniformName) == 0)
			{
			return ShaderInformation->Uniforms[Index].Handle;
			}
		}
	for (unsigned Index = 0; Index < ShaderInformation->UniformBlockCount; ++Index)
		{
		if (strcmp(ShaderInformation->UniformBlocks[Index].Name, UniformName) == 0)
			{
			return ShaderInformation->UniformBlocks[Index].Handle;
			}
		}
	return -1;
	}

crShaderAttributeHandle crGetShaderAttributeHandle(const crShaderHandle ShaderHandle, const char *AttributeName)
	{
	const crShaderInformation *ShaderInformation = crGetShaderInformation(ShaderHandle);
	if (ShaderInformation == NULL)
		return -1;
	for (unsigned Index = 0; Index < ShaderInformation->AttributeCount; ++Index)
		{
		if (strcmp(ShaderInformation->Attributes[Index].Name, AttributeName) == 0)
			{
			return ShaderInformation->Attributes[Index].Handle;
			}
		}
	return -1;
	}
