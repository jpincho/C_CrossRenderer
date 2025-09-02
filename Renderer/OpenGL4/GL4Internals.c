#include "GL4Internals.h"
#include "../Internal/Logger.h"

OpenGLInformationStruct OpenGLInformation;

static void FatalError(const unsigned Line, const char *Filename, const char *Message)
	{
	printf("FATAL ERROR at line %u ( %s ) - %s\n", Line, Filename, Message);
	exit(-1);
	}
#define FATAL_ERROR(MSG) {FatalError(__LINE__, __FILE__, MSG);return false;}

void OpenGLMessageCallback(GLenum Source, GLenum Type, GLuint ID, GLenum Severity, GLsizei Length, GLchar const *Message, void const *UserParam)
	{
	if ((Type == GL_DEBUG_TYPE_OTHER) && (Severity == GL_DEBUG_SEVERITY_NOTIFICATION))
		return;
	if (Type == GL_DEBUG_TYPE_ERROR)
		LOG_ERROR("%s - %s - %s - %X - %s", StringifyOpenGL(Source), StringifyOpenGL(Type), StringifyOpenGL(Severity), ID, Message);
	else
		LOG_DEBUG("%s - %s - %s - %X - %s", StringifyOpenGL(Source), StringifyOpenGL(Type), StringifyOpenGL(Severity), ID, Message);
	}

const char *StringifyOpenGL(GLenum Value)
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

bool CheckError(void)
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
			snprintf(ErrorString, 100, "OpenGL error - %s", StringifyOpenGL(Error));
			LOG_ERROR(ErrorString);
			}
		} while (Error != GL_NO_ERROR);
	return Result;
#endif
	}

#if 0
bool TranslateDepthTestMode(const DepthTestMode input, GLenum *output)
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
	static unsigned Count = sizeof(Values) / sizeof(GLenum);
	if (static_cast <unsigned> (Value) >= Count)
		throw std::runtime_error(std::string("Invalid depth function"));
	return Values[static_cast <unsigned> (Value)];
	}

GLenum Translate(const BlendMode Value)
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
	static unsigned Count = sizeof(Values) / sizeof(GLenum);
	if (static_cast <unsigned> (Value) >= Count)
		throw std::runtime_error(std::string("Invalid blend mode"));
	return Values[static_cast <unsigned> (Value)];
	}

GLenum Translate(const StencilFunction Value)
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
	static unsigned Count = sizeof(Values) / sizeof(GLenum);
	if (static_cast <unsigned> (Value) >= Count)
		throw std::runtime_error(std::string("Invalid stencil function"));
	return Values[static_cast <unsigned> (Value)];
	}

GLenum Translate(const StencilFailAction Value)
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
	static unsigned Count = sizeof(Values) / sizeof(GLenum);
	if (static_cast <unsigned> (Value) >= Count)
		throw std::runtime_error(std::string("Invalid stencil fail action"));
	return Values[static_cast <unsigned> (Value)];
	}

GLenum Translate(const CullingMode Value)
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
	static unsigned Count = sizeof(Values) / sizeof(GLenum);
	if (static_cast <unsigned> (Value) >= Count)
		throw std::runtime_error(std::string("Invalid culling mode"));
	return Values[static_cast <unsigned> (Value)];
	}

GLenum Translate(const CullingFaceWinding Value)
	{
	static GLenum Values[] =
		{
		GL_CW,
		GL_CCW,
		};
	static unsigned Count = sizeof(Values) / sizeof(GLenum);
	if (static_cast <unsigned> (Value) >= Count)
		throw std::runtime_error(std::string("Invalid winding mode"));
	return Values[static_cast <unsigned> (Value)];
	}

GLenum Translate(const ShaderBufferComponentType Value)
	{
	static GLenum Values[] =
		{
		GL_FLOAT,
		GL_UNSIGNED_BYTE,
		GL_UNSIGNED_SHORT,
		GL_UNSIGNED_INT,
		};
	static unsigned Count = sizeof(Values) / sizeof(GLenum);
	if (static_cast <unsigned> (Value) >= Count)
		throw std::runtime_error(std::string("Invalid component type"));
	return Values[static_cast <unsigned> (Value)];
	}

GLenum Translate(const ShaderBufferAccessType Value)
	{
	static GLenum Values[] =
		{
		GL_STATIC_DRAW,
		GL_DYNAMIC_DRAW,
		GL_STREAM_DRAW
		};
	static unsigned Count = sizeof(Values) / sizeof(GLenum);
	if (static_cast <unsigned> (Value) >= Count)
		throw std::runtime_error(std::string("Invalid access type"));
	return Values[static_cast <unsigned> (Value)];
	}

GLenum Translate(const ShaderBufferMapAccessType Value)
	{
	static GLenum Values[] =
		{
		GL_READ_ONLY,
		GL_WRITE_ONLY,
		GL_READ_WRITE
		};
	static unsigned Count = sizeof(Values) / sizeof(GLenum);
	if (static_cast <unsigned> (Value) >= Count)
		throw std::runtime_error(std::string("Invalid map access type"));
	return Values[static_cast <unsigned> (Value)];
	}

GLenum Translate(const ShaderBufferType Value)
	{
	static GLenum Values[] =
		{
		GL_ARRAY_BUFFER,
		GL_UNIFORM_BUFFER,
		};
	static unsigned Count = sizeof(Values) / sizeof(GLenum);
	if (static_cast <unsigned> (Value) >= Count)
		throw std::runtime_error(std::string("Invalid buffer type"));
	return Values[static_cast <unsigned> (Value)];
	}

GLenum Translate(const PrimitiveType Value)
	{
	static GLenum Values[] =
		{
		GL_POINTS,
		GL_LINES,
		GL_LINE_STRIP,
		GL_TRIANGLES,
		GL_TRIANGLE_STRIP
		};
	static unsigned Count = sizeof(Values) / sizeof(GLenum);
	if (static_cast <unsigned> (Value) >= Count)
		throw std::runtime_error(std::string("Invalid primitive type"));
	return Values[static_cast <unsigned> (Value)];
	}
#endif
bool TranslateTextureFilter(const ETextureFilter Input, GLint *Output)
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
	static unsigned Count = sizeof(Values) / sizeof(GLenum);
	if (Input >= Count)
		FATAL_ERROR("Invalid texture filter");
	*Output = Values[Input]; 
	return true;
	}

bool TranslateTextureWrapMode(const ETextureWrapMode Input, GLint *Output)
	{
	static GLint Values[] =
		{
		GL_REPEAT,
		GL_MIRRORED_REPEAT,
		GL_CLAMP_TO_EDGE,
		GL_CLAMP_TO_BORDER
		};
	static unsigned Count = sizeof(Values) / sizeof(GLenum);
	if (Input >= Count)
		FATAL_ERROR("Invalid texture wrap mode");
	*Output = Values[Input];
	return true;
	}

bool TranslateTextureType(const ETextureType Input, GLenum *Output)
	{
	static GLenum Values[] =
		{
		GL_TEXTURE_2D,
		GL_TEXTURE_CUBE_MAP
		};
	static unsigned Count = sizeof(Values) / sizeof(GLenum);
	if (Input >= Count)
		FATAL_ERROR("Invalid texture type");
	*Output = Values[Input];
	return true;
	}
#if 0
GLenum Translate(const PolygonMode Value)
	{
	static GLenum Values[] =
		{
		GL_POINT,
		GL_LINE,
		GL_FILL
		};
	static unsigned Count = sizeof(Values) / sizeof(GLenum);
	if (static_cast <unsigned> (Value) >= Count)
		throw std::runtime_error(std::string("Invalid polygon mode"));
	return Values[static_cast <unsigned> (Value)];
	}
#endif