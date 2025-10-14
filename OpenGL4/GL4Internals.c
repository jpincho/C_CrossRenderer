#include "GL4Internals.h"

struct OpenGLInformationStruct crGL4Information;

#define FATAL_ERROR(MSG) {printf("FATAL ERROR at line %u ( %s ) - %s\n", __LINE__, __FILE__, MSG);exit(-1);}

void OpenGLMessageCallback(GLenum Source, GLenum Type, GLuint ID, GLenum Severity, GLsizei Length, GLchar const *Message, void const *UserParam)
	{
	UNUSED(UserParam);
	UNUSED(Length);
	if ((Type == GL_DEBUG_TYPE_OTHER) && (Severity == GL_DEBUG_SEVERITY_NOTIFICATION))
		return;
	if (Type == GL_DEBUG_TYPE_ERROR)
		LOG_ERROR("%s - %s - %s - %X - %s", crGL4StringifyOpenGL(Source), crGL4StringifyOpenGL(Type), crGL4StringifyOpenGL(Severity), ID, Message);
	else
		LOG_DEBUG("%s - %s - %s - %X - %s", crGL4StringifyOpenGL(Source), crGL4StringifyOpenGL(Type), crGL4StringifyOpenGL(Severity), ID, Message);
	}

const char *crGL4StringifyOpenGL(GLenum Value)
	{
	switch (Value)
		{
#define STRINGIFY(X) case X: return #X;
		STRINGIFY(GL_FRAMEBUFFER_UNDEFINED);
		STRINGIFY(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
		STRINGIFY(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
		STRINGIFY(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER);
		STRINGIFY(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER);
		STRINGIFY(GL_FRAMEBUFFER_UNSUPPORTED);
		STRINGIFY(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
		STRINGIFY(GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS);
		STRINGIFY(GL_FRAMEBUFFER_COMPLETE);

		STRINGIFY(GL_NO_ERROR);
		STRINGIFY(GL_INVALID_ENUM);
		STRINGIFY(GL_INVALID_VALUE);
		STRINGIFY(GL_INVALID_OPERATION);
		STRINGIFY(GL_STACK_OVERFLOW);
		STRINGIFY(GL_STACK_UNDERFLOW);
		STRINGIFY(GL_OUT_OF_MEMORY);
		STRINGIFY(GL_TABLE_TOO_LARGE);

		STRINGIFY(GL_DEBUG_SOURCE_API);
		STRINGIFY(GL_DEBUG_SOURCE_WINDOW_SYSTEM);
		STRINGIFY(GL_DEBUG_SOURCE_SHADER_COMPILER);
		STRINGIFY(GL_DEBUG_SOURCE_THIRD_PARTY);
		STRINGIFY(GL_DEBUG_SOURCE_APPLICATION);
		STRINGIFY(GL_DEBUG_SOURCE_OTHER);

		STRINGIFY(GL_DEBUG_TYPE_ERROR);
		STRINGIFY(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR);
		STRINGIFY(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR);
		STRINGIFY(GL_DEBUG_TYPE_PORTABILITY);
		STRINGIFY(GL_DEBUG_TYPE_PERFORMANCE);
		STRINGIFY(GL_DEBUG_TYPE_MARKER);
		STRINGIFY(GL_DEBUG_TYPE_OTHER);

		STRINGIFY(GL_DEBUG_SEVERITY_NOTIFICATION);
		STRINGIFY(GL_DEBUG_SEVERITY_LOW);
		STRINGIFY(GL_DEBUG_SEVERITY_MEDIUM);
		STRINGIFY(GL_DEBUG_SEVERITY_HIGH);

		default:
			FATAL_ERROR("Unhandled OpenGL enum");
		}
#undef STRINGIFY
	}

bool crGL4CheckError(void)
	{
#if defined ( CROSS_RENDERER_OPENGL_DISABLE_CHECK_ERROR )
	return true;
#else
	GLenum Error;
	bool Result = true;
	do
		{
		Error = glGetError();
		if (Error != GL_NO_ERROR)
			{
			Result = false;
			static char ErrorString[100];
			snprintf(ErrorString, 100, "OpenGL error - %s", crGL4StringifyOpenGL(Error));
			LOG_ERROR(ErrorString);
			}
		} while (Error != GL_NO_ERROR);
	return Result;
#endif
	}

GLenum crGL4TranslateDepthTestMode(const enum crDepthTestMode Input)
	{
	static GLenum Values[] =
		{
		GL_NEVER,
		GL_LESS,
		GL_LEQUAL,
		GL_GREATER,
		GL_GEQUAL,
		GL_EQUAL,
		GL_NOTEQUAL,
		GL_ALWAYS
		};
	static int Count = sizeof(Values) / sizeof(GLenum);
	if ((Input >= Count) || (Input < 0))
		FATAL_ERROR("Invalid depth test mode");
	return Values[Input];
	}


GLenum crGL4TranslateBlendMode(const enum crBlendMode Input)
	{
	static GLenum Values[] =
		{
		GL_ZERO,
		GL_ONE,
		GL_SRC_COLOR,
		GL_ONE_MINUS_SRC_COLOR,
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_DST_ALPHA,
		GL_ONE_MINUS_DST_ALPHA,
		GL_DST_COLOR,
		GL_ONE_MINUS_DST_COLOR,
		GL_SRC_ALPHA_SATURATE
		};
	static int Count = sizeof(Values) / sizeof(GLenum);
	if ((Input >= Count) || (Input < 0))
		FATAL_ERROR("Invalid blend mode");
	return Values[Input];
	}

GLenum crGL4TranslateStencilFunction(const enum crStencilFunction Input)
	{
	static GLenum Values[] =
		{
		GL_NEVER,
		GL_LESS,
		GL_LEQUAL,
		GL_GREATER,
		GL_GEQUAL,
		GL_EQUAL,
		GL_NOTEQUAL,
		GL_ALWAYS
		};
	static int Count = sizeof(Values) / sizeof(GLenum);
	if ((Input >= Count) || (Input < 0))
		FATAL_ERROR("Invalid stencil function");
	return Values[Input];
	}

GLenum crGL4TranslateStencilFailAction(const enum crStencilFailAction Input)
	{
	static GLenum Values[] =
		{
		GL_ZERO,
		GL_KEEP,
		GL_REPLACE,
		GL_INCR,
		GL_DECR,
		GL_INVERT
		};
	static int Count = sizeof(Values) / sizeof(GLenum);
	if ((Input >= Count) || (Input < 0))
		FATAL_ERROR("Invalid stencil fail action");
	return Values[Input];
	}

GLenum crGL4TranslateCullingMode(const enum crCullingMode Input)
	{
	static GLenum Values[] =
		{
		GL_FRONT,
		GL_BACK,
		GL_FRONT_AND_BACK,
		GL_INCR,
		GL_DECR,
		GL_INVERT
		};
	static int Count = sizeof(Values) / sizeof(GLenum);
	if ((Input >= Count) || (Input < 0))
		FATAL_ERROR("Invalid culling mode");
	return Values[Input];
	}

GLenum crGL4TranslateCullingFaceWinding(const enum crCullingFaceWinding Input)
	{
	static GLenum Values[] =
		{
		GL_CW,
		GL_CCW,
		};
	static int Count = sizeof(Values) / sizeof(GLenum);
	if ((Input >= Count) || (Input < 0))
		FATAL_ERROR("Invalid culling winding");
	return Values[Input];
	}

GLenum crGL4TranslatePrimitive(const enum crPrimitiveType Input)
	{
	static GLenum Values[] =
		{
		GL_POINTS,
		GL_LINES,
		GL_LINE_STRIP,
		GL_TRIANGLES,
		GL_TRIANGLE_STRIP
		};
	static int Count = sizeof(Values) / sizeof(GLenum);
	if ((Input >= Count) || (Input < 0))
		FATAL_ERROR("Invalid primitive");
	return Values[Input];
	}

GLenum crGL4TranslateTextureFilter(const enum crTextureFilter Input)
	{
	static GLint Values[] =
		{
		GL_LINEAR,
		GL_NEAREST,
		GL_NEAREST_MIPMAP_NEAREST,
		GL_NEAREST_MIPMAP_LINEAR,
		GL_LINEAR_MIPMAP_LINEAR,
		GL_LINEAR_MIPMAP_NEAREST
		};
	static int Count = sizeof(Values) / sizeof(GLenum);
	if ((Input >= Count) || (Input < 0))
		FATAL_ERROR("Invalid texture filter");
	return Values[Input];
	}

GLenum crGL4TranslateTextureWrapMode(const enum crTextureWrapMode Input)
	{
	static GLint Values[] =
		{
		GL_REPEAT,
		GL_MIRRORED_REPEAT,
		GL_CLAMP_TO_EDGE,
		GL_CLAMP_TO_BORDER
		};
	static int Count = sizeof(Values) / sizeof(GLenum);
	if ((Input >= Count) || (Input < 0))
		FATAL_ERROR("Invalid texture wrap mode");
	return Values[Input];
	}

GLenum crGL4TranslateTextureType(const enum crTextureType Input)
	{
	static GLenum Values[] =
		{
		GL_TEXTURE_2D,
		GL_TEXTURE_CUBE_MAP
		};
	static int Count = sizeof(Values) / sizeof(GLenum);
	if ((Input >= Count) || (Input < 0))
		FATAL_ERROR("Invalid texture type");
	return Values[Input];
	}

GLenum crGL4TranslateShaderBufferComponentType(const enum crShaderBufferComponentType Input)
	{
	static GLenum Values[] =
		{
		GL_FLOAT,
		GL_UNSIGNED_BYTE,
		GL_UNSIGNED_SHORT,
		GL_UNSIGNED_INT,
		};
	static int Count = sizeof(Values) / sizeof(GLenum);
	if ((Input >= Count) || (Input < 0))
		FATAL_ERROR("Invalid shader buffer component type");
	return Values[Input];
	}

GLenum crGL4TranslateShaderBufferAccessType(const enum crShaderBufferAccessType Input)
	{
	static GLenum Values[] =
		{
		GL_STATIC_DRAW,
		GL_DYNAMIC_DRAW,
		GL_STREAM_DRAW
		};
	static int Count = sizeof(Values) / sizeof(GLenum);
	if ((Input >= Count) || (Input < 0))
		FATAL_ERROR("Invalid shader buffer access type");
	return Values[Input];
	}

GLenum crGL4TranslateShaderBufferMapAccessType(const enum crShaderBufferMapAccessType Input)
	{
	static GLenum Values[] =
		{
		GL_READ_ONLY,
		GL_WRITE_ONLY,
		GL_READ_WRITE
		};
	static int Count = sizeof(Values) / sizeof(GLenum);
	if ((Input >= Count) || (Input < 0))
		FATAL_ERROR("Invalid shader buffer type");
	return Values[Input];
	}

GLenum crGL4TranslateShaderBufferType(const enum crShaderBufferType Input)
	{
	static GLenum Values[] =
		{
		GL_ARRAY_BUFFER,
		GL_UNIFORM_BUFFER,
		};
	static int Count = sizeof(Values) / sizeof(GLenum);
	if ((Input >= Count) || (Input < 0))
		FATAL_ERROR("Invalid shader buffer type");
	return Values[Input];
	}

GLenum crGL4TranslatePolygonMode(const enum crPolygonMode Input)
	{
	static GLenum Values[] =
		{
		GL_POINT,
		GL_LINE,
		GL_FILL
		};
	static int Count = sizeof(Values) / sizeof(GLenum);
	if ((Input >= Count) || (Input < 0))
		FATAL_ERROR("Invalid polygon mode");
	return Values[Input];
	}

enum crShaderUniformType crGL4TranslateOpenGLUniformType(const GLenum Input)
	{
	switch (Input)
		{
		case GL_FLOAT:
			return crShaderUniformType_Float;
		case GL_FLOAT_VEC2:
			return crShaderUniformType_Float2;
		case GL_FLOAT_VEC3:
			return crShaderUniformType_Float3;
		case GL_FLOAT_VEC4:
			return crShaderUniformType_Float4;

		case GL_INT:
			return crShaderUniformType_Integer;
		case GL_INT_VEC2:
			return crShaderUniformType_Integer2;
		case GL_INT_VEC3:
			return crShaderUniformType_Integer3;
		case GL_INT_VEC4:
			return crShaderUniformType_Integer4;

		case GL_UNSIGNED_INT:
			return crShaderUniformType_UnsignedInteger;
#if defined GL_UNSIGNED_INT_VEC2
		case GL_UNSIGNED_INT_VEC2:
			return crShaderUniformType_UnsignedInteger2;
		case GL_UNSIGNED_INT_VEC3:
			return crShaderUniformType_UnsignedInteger3;
		case GL_UNSIGNED_INT_VEC4:
			return crShaderUniformType_UnsignedInteger4;
#endif
		case GL_BOOL:
			return crShaderUniformType_Bool;
		case GL_BOOL_VEC2:
			return crShaderUniformType_Bool2;
		case GL_BOOL_VEC3:
			return crShaderUniformType_Bool3;
		case GL_BOOL_VEC4:
			return crShaderUniformType_Bool4;

		case GL_FLOAT_MAT2:
			return crShaderUniformType_Matrix2;
		case GL_FLOAT_MAT3:
			return crShaderUniformType_Matrix3;
		case GL_FLOAT_MAT4:
			return crShaderUniformType_Matrix4;
		case GL_SAMPLER_2D:
			return crShaderUniformType_Sampler2D;
		case GL_SAMPLER_3D:
			return crShaderUniformType_Sampler3D;
		case GL_SAMPLER_CUBE:
			return crShaderUniformType_SamplerCube;
		default:
			LOG_ERROR("Unhandled shader uniform type");
			return crShaderUniformType_Unknown;
		}
	}
