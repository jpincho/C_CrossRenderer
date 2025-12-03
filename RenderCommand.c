#include "RenderCommand.h"
#include "Internal/CrossRendererFunctionPointers.h"
#include <Platform/Logger.h>
#include "Shader.h"
#include <assert.h>
#include <string.h>

void crSetRenderCommandShader(crRenderCommand *Command, const crShaderHandle NewShader)
	{
	SAFE_DEL_C(Command->UniformValues);
	SAFE_DEL_C(Command->ShaderBufferBindings);
	SAFE_DEL_C(Command->ShaderTextureBindings);
	Command->Shader = NewShader;
	const crShaderInformation *ShaderInformation = crGetShaderInformation(NewShader);
	Command->UniformValues = calloc(ShaderInformation->UniformCount, sizeof(crShaderUniformValuePair));
	Command->UniformValueCount = 0;// ShaderInformation->UniformCount;
	Command->ShaderBufferBindings = calloc(ShaderInformation->AttributeCount, sizeof(crShaderBufferBindPair));
	Command->ShaderBufferBindingCount = 0;// ShaderInformation->AttributeCount;
	Command->ShaderTextureBindings = calloc(ShaderInformation->UniformCount, sizeof(crShaderTextureBindPair));
	Command->ShaderTextureBindingCount = 0;// ShaderInformation->AttributeCount;
	}

bool crSetRenderCommandIndexShaderBufferBinding(crRenderCommand *Command, const crShaderBufferDataStream Stream)
	{
	Command->IndexBufferStream = Stream;
	return true;
	}

bool crSetRenderCommandShaderBufferBinding(crRenderCommand *Command, const crShaderAttributeHandle AttributeHandle, const crShaderBufferDataStream Stream)
	{
	//const crShaderInformation *ShaderInformation = crGetShaderInformation(Command->Shader);

	if (AttributeHandle == -1)
		return false;

	// Check if a binding is already present. If so, replace it
	int IndexToUse = -1;
	for (unsigned BindingIndex = 0; BindingIndex < Command->ShaderBufferBindingCount; ++BindingIndex)
		{
		if (Command->ShaderBufferBindings[BindingIndex].AttributeHandle == AttributeHandle)
			{
			IndexToUse = BindingIndex;
			break;
			}
		}

	if (IndexToUse == -1) // Not found. Add this to the array
		{
		IndexToUse = Command->ShaderBufferBindingCount;
		++Command->ShaderBufferBindingCount;
		}

	Command->ShaderBufferBindings[IndexToUse].AttributeHandle = AttributeHandle;
	Command->ShaderBufferBindings[IndexToUse].DataStream = Stream;
	return true;
	}

bool crSetRenderCommandTextureBinding(crRenderCommand *Command, const crShaderUniformHandle UniformHandle, const crTextureBindSettings Binding)
	{
	int IndexToUse = -1;
	for (unsigned ValueIndex = 0; ValueIndex < Command->ShaderTextureBindingCount; ++ValueIndex)
		{
		if (Command->ShaderTextureBindings[ValueIndex].UniformHandle == UniformHandle)
			{
			IndexToUse = ValueIndex;
			break;
			}
		}

	if (IndexToUse == -1)
		{
		IndexToUse = Command->ShaderTextureBindingCount;
		++Command->ShaderTextureBindingCount;
		}
	Command->ShaderTextureBindings[IndexToUse].UniformHandle = UniformHandle;
	Command->ShaderTextureBindings[IndexToUse].BindSettings = Binding;
	return true;
	}

static inline int FindUniformInRenderCommand(crRenderCommand *Command, const crShaderUniformHandle UniformHandle)
	{
	for (unsigned ValueIndex = 0; ValueIndex < Command->UniformValueCount; ++ValueIndex)
		{
		if (Command->UniformValues[ValueIndex].UniformHandle == UniformHandle)
			{
			return ValueIndex;
			}
		}

	return Command->UniformValueCount++;
	}

#define IMPLEMENT_crSetRenderCommandUniformValue(VAR_TYPE,VALUE_TYPE) \
bool crSetRenderCommandUniform##VALUE_TYPE##Value(crRenderCommand *Command, const crShaderUniformHandle UniformHandle, const VAR_TYPE Value)\
	{\
	if (UniformHandle == -1)\
		return false;\
\
	int IndexToUse = FindUniformInRenderCommand(Command, UniformHandle);\
\
	Command->UniformValues[IndexToUse].UniformHandle = UniformHandle;\
	Command->UniformValues[IndexToUse].UniformValue.VALUE_TYPE##Value = Value;\
	return true;\
	}

IMPLEMENT_crSetRenderCommandUniformValue(bool, Bool);
IMPLEMENT_crSetRenderCommandUniformValue(float, Float);
IMPLEMENT_crSetRenderCommandUniformValue(unsigned, UnsignedInteger);
IMPLEMENT_crSetRenderCommandUniformValue(int, Integer);
IMPLEMENT_crSetRenderCommandUniformValue(crShaderBufferHandle, Block);
#undef IMPLEMENT_crSetRenderCommandUniformValue

#define IMPLEMENT_crSetRenderCommandUniformValue(VAR_TYPE,VALUE_TYPE) \
bool crSetRenderCommandUniform##VALUE_TYPE##Value(crRenderCommand *Command, const crShaderUniformHandle UniformHandle, const VAR_TYPE Value)\
	{\
	if (UniformHandle == -1)\
		return false;\
\
	int IndexToUse = FindUniformInRenderCommand(Command, UniformHandle);\
\
	Command->UniformValues[IndexToUse].UniformHandle = UniformHandle;\
	math_##VAR_TYPE##_copy(&Command->UniformValues[IndexToUse].UniformValue.VALUE_TYPE##Value, Value);\
	return true;\
	}

IMPLEMENT_crSetRenderCommandUniformValue(ivec2, Bool2);
IMPLEMENT_crSetRenderCommandUniformValue(ivec3, Bool3);
IMPLEMENT_crSetRenderCommandUniformValue(ivec4, Bool4);
IMPLEMENT_crSetRenderCommandUniformValue(vec2, Float2);
IMPLEMENT_crSetRenderCommandUniformValue(vec3, Float3);
IMPLEMENT_crSetRenderCommandUniformValue(vec4, Float4);
IMPLEMENT_crSetRenderCommandUniformValue(uvec2, UnsignedInteger2);
IMPLEMENT_crSetRenderCommandUniformValue(uvec3, UnsignedInteger3);
IMPLEMENT_crSetRenderCommandUniformValue(uvec4, UnsignedInteger4);
IMPLEMENT_crSetRenderCommandUniformValue(ivec2, Integer2);
IMPLEMENT_crSetRenderCommandUniformValue(ivec3, Integer3);
IMPLEMENT_crSetRenderCommandUniformValue(ivec4, Integer4);
IMPLEMENT_crSetRenderCommandUniformValue(mat2, Matrix2);
IMPLEMENT_crSetRenderCommandUniformValue(mat3, Matrix3);
IMPLEMENT_crSetRenderCommandUniformValue(mat4, Matrix4);
#undef IMPLEMENT_crSetRenderCommandUniformValue

void crDestroyRenderCommand(crRenderCommand *Command)
	{
	SAFE_DEL_C(Command->UniformValues);
	SAFE_DEL_C(Command->ShaderBufferBindings);
	SAFE_DEL_C(Command->ShaderTextureBindings);
	memset(Command, 0, sizeof(crRenderCommand));
	}