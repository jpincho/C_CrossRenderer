#pragma once
#include <Platform/Platform.h>
#include <Platform/defines.h>
#include <Platform/Version.h>
#include <Platform/Logger.h>
#include <glad/glad.h>
#include <C_Utils/pointer_list.h>

#include "../CrossRenderer.h"

struct OpenGLInformationStruct
	{
	GLint MaxTextureUnits;
	struct version OpenGLVersion, GLSLVersion;
	bool DirectStateAccessEnabled, DirectStateAccessAvailable;
	bool SeamlessCubeMapEnabled, SeamlessCubeMapAvailable;
	char **Extensions;
	unsigned ExtensionCount;
	pointer_list Framebuffers;
	pointer_list Textures;
	pointer_list ShaderBuffers;
	pointer_list Shaders;
	};

extern struct OpenGLInformationStruct crGL4Information;

struct crGL4InternalTextureInfo
	{
	uvec2 Dimensions;
	GLuint OpenGLID;
	GLint GLMinFilter, GLMagFilter, GLSWrap, GLTWrap;
	GLenum GLTextureType;
	bool Mipmapped;
	enum crPixelFormat Format;
	};

struct crGL4InternalShaderBufferInfo
	{
	GLuint OpenGLID;
	GLenum GLAccessType;
	GLenum GLBufferType;
	GLenum GLMappedAccessType;
	void *MappedPointer;
	size_t DataSize, Capacity;
	};

struct crGL4InternalFramebufferInfo
	{
	uvec2 Dimensions;
	GLuint OpenGLID;
	crTextureHandle *ColorTextures;
	unsigned ColorTextureCount;
	crTextureHandle DepthTexture;
	crTextureHandle StencilTexture;
	vec4 ClearColor;
	float ClearDepth;
	};

struct crGL4InternalUniformInfo
	{
	GLint OpenGLID;
	struct crShaderUniformValue CurrentValue;
	enum crShaderUniformType Type;
	char *Name;
	};

struct crGL4InternalAttributeInfo
	{
	GLint OpenGLID;
	bool Enabled;
	enum crShaderAttributeType Type;
	char *Name;
	};

struct crGL4InternalShaderInfo
	{
	GLuint OpenGLID;
	struct crGL4InternalUniformInfo *Uniforms;
	unsigned UniformCount;
	struct crGL4InternalUniformInfo *UniformBlocks;
	unsigned UniformBlockCount;
	struct crGL4InternalAttributeInfo *Attributes;
	unsigned AttributeCount;

	struct crShaderInformation GeneralInformation;
	};

void OpenGLMessageCallback ( GLenum Source, GLenum Type, GLuint ID, GLenum Severity, GLsizei Length, GLchar const *Message, void const *UserParam );
const char *crGL4StringifyOpenGL ( GLenum Value );
bool crGL4CheckError ( void );

GLenum crGL4TranslateDepthTestMode ( const enum crDepthTestMode Input );
GLenum crGL4TranslateBlendMode ( const enum crBlendMode Input );
GLenum crGL4TranslateStencilFunction ( const enum crStencilFunction Input );
GLenum crGL4TranslateStencilFailAction ( const enum crStencilFailAction Input );
GLenum crGL4TranslateCullingMode ( const enum crCullingMode Input );
GLenum crGL4TranslateCullingFaceWinding ( const enum crCullingFaceWinding Input );
GLenum crGL4TranslatePrimitive ( const enum crPrimitiveType Input );
GLenum crGL4TranslateTextureFilter ( const enum crTextureFilter Input );
GLenum crGL4TranslateTextureWrapMode ( const enum crTextureWrapMode Input );
GLenum crGL4TranslateTextureType ( const enum crTextureType Input );
GLenum crGL4TranslateShaderBufferComponentType ( const enum crShaderBufferComponentType Input );
GLenum crGL4TranslateShaderBufferAccessType ( const enum crShaderBufferAccessType Input );
GLenum crGL4TranslateShaderBufferMapAccessType ( const enum crShaderBufferMapAccessType Input );
GLenum crGL4TranslateShaderBufferType ( const enum crShaderBufferType Input );
GLenum crGL4TranslatePolygonMode ( const enum crPolygonMode Input );
enum crShaderUniformType crGL4TranslateOpenGLUniformType ( const GLenum Input );
