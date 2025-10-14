#pragma once
#include <Platform/defines.h>
#include "Shader.h"
#include "ShaderBuffer.h"
#include "RenderStateDefinitions.h"
#include "Texture.h"

BEGIN_C_DECLARATIONS

enum crPrimitiveType
	{
	crPoints = 0,
	crLineList,
	crLineStrip,
	crTriangleList,
	crTriangleStrip
	};

struct crShaderTextureBindPair
	{
	crShaderUniformHandle UniformHandle;
	struct crTextureBindSettings BindSettings;
	};

struct crShaderBufferBindPair
	{
	crShaderAttributeHandle AttributeHandle;
	struct crShaderBufferDataStream DataStream;
	};

struct crShaderUniformValuePair
	{
	crShaderUniformHandle UniformHandle;
	struct crShaderUniformValue UniformValue;
	};

struct crRenderCommand
	{
	crShaderHandle Shader;
	struct crShaderUniformValuePair *UniformValues;
	unsigned UniformValueCount;
	struct crShaderBufferBindPair *ShaderBufferBindings;
	unsigned ShaderBufferBindingCount;
	struct crShaderTextureBindPair *ShaderTextureBindings;
	unsigned ShaderTextureBindingCount;

	struct crShaderBufferDataStream IndexBufferStream;
	enum crPrimitiveType Primitive;
	unsigned VertexCount, StartVertex;

	size_t InstanceCount;
	struct crRenderState State;
	};

void crSetRenderCommandShader ( struct crRenderCommand *Command, const crShaderHandle NewShader );
bool crSetRenderCommandIndexShaderBufferBinding ( struct crRenderCommand *Command, const struct crShaderBufferDataStream Stream );
bool crSetRenderCommandShaderBufferBinding( struct crRenderCommand *Command, const crShaderAttributeHandle AttributeHandle, const struct crShaderBufferDataStream Stream );
#define DEFINE_crSetRenderCommandUniformValue(VAR_TYPE,VALUE_TYPE) bool crSetRenderCommandUniform##VALUE_TYPE##Value(struct crRenderCommand *Command, const int UniformIndex, const VAR_TYPE Value);

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
#undef DEFINE_crSetRenderCommandUniformValue
END_C_DECLARATIONS

bool crSetRenderCommandTextureBinding ( struct crRenderCommand *Command, const crShaderUniformHandle UniformHandle, const struct crTextureBindSettings Binding );

