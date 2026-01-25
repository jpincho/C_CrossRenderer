#pragma once
#include <Platform/defines.h>
#include "Shader.h"
#include "ShaderBuffer.h"
#include "RenderStateDefinitions.h"
#include "Texture.h"

BEGIN_C_DECLARATIONS
typedef enum
	{
	crPoints = 0,
	crLineList,
	crLineStrip,
	crTriangleList,
	crTriangleStrip
	} crPrimitiveType;

typedef struct
	{
	crShaderUniformHandle UniformHandle;
	crTextureBindSettings BindSettings;
	} crShaderTextureBindPair;

typedef struct
	{
	crShaderAttributeHandle AttributeHandle;
	crShaderBufferDataStream DataStream;
	} crShaderBufferBindPair;

typedef struct
	{
	crShaderUniformHandle UniformHandle;
	crShaderUniformValue UniformValue;
	} crShaderUniformValuePair;

typedef struct
	{
	crShaderHandle Shader;
	crShaderUniformValuePair *UniformValues;
	unsigned UniformValueCount;
	crShaderBufferBindPair *ShaderBufferBindings;
	unsigned ShaderBufferBindingCount;
	crShaderTextureBindPair *ShaderTextureBindings;
	unsigned ShaderTextureBindingCount;

	crShaderBufferDataStream IndexBufferStream;
	crPrimitiveType Primitive;
	unsigned VertexCount, StartVertex;

	size_t InstanceCount;
	crRenderState State;
	} crRenderCommand;

void crSetRenderCommandShader ( crRenderCommand *Command, const crShaderHandle NewShader );
bool crSetRenderCommandIndexShaderBufferBinding ( crRenderCommand *Command, const crShaderBufferDataStream Stream );
bool crSetRenderCommandShaderBufferBinding ( crRenderCommand *Command, const crShaderAttributeHandle AttributeHandle, const crShaderBufferDataStream Stream );
bool crSetRenderCommandTextureBinding ( crRenderCommand *Command, const crShaderUniformHandle UniformHandle, const crTextureBindSettings Binding );
#define DEFINE_crSetRenderCommandUniformValue(VAR_TYPE,VALUE_TYPE) bool crSetRenderCommandUniform##VALUE_TYPE##Value(crRenderCommand *Command, const int UniformIndex, const VAR_TYPE Value);

DEFINE_crSetRenderCommandUniformValue ( bool, Bool );
DEFINE_crSetRenderCommandUniformValue ( ivec2, Bool2 );
DEFINE_crSetRenderCommandUniformValue ( ivec3, Bool3 );
DEFINE_crSetRenderCommandUniformValue ( ivec4, Bool4 );
DEFINE_crSetRenderCommandUniformValue ( float, Float );
DEFINE_crSetRenderCommandUniformValue ( vec2, Float2 );
DEFINE_crSetRenderCommandUniformValue ( vec3, Float3 );
DEFINE_crSetRenderCommandUniformValue ( vec4, Float4 );
DEFINE_crSetRenderCommandUniformValue ( unsigned, UnsignedInteger );
DEFINE_crSetRenderCommandUniformValue ( uvec2, UnsignedInteger2 );
DEFINE_crSetRenderCommandUniformValue ( uvec3, UnsignedInteger3 );
DEFINE_crSetRenderCommandUniformValue ( uvec4, UnsignedInteger4 );
DEFINE_crSetRenderCommandUniformValue ( int, Integer );
DEFINE_crSetRenderCommandUniformValue ( ivec2, Integer2 );
DEFINE_crSetRenderCommandUniformValue ( ivec3, Integer3 );
DEFINE_crSetRenderCommandUniformValue ( ivec4, Integer4 );
DEFINE_crSetRenderCommandUniformValue ( mat2, Matrix2 );
DEFINE_crSetRenderCommandUniformValue ( mat3, Matrix3 );
DEFINE_crSetRenderCommandUniformValue ( mat4, Matrix4 );
DEFINE_crSetRenderCommandUniformValue ( crShaderBufferHandle, Block );
#undef DEFINE_crSetRenderCommandUniformValue

void crDestroyRenderCommand ( crRenderCommand *Command );
END_C_DECLARATIONS
