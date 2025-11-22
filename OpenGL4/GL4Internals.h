#pragma once
#include <Platform/Platform.h>
#include <Platform/defines.h>
#include <Platform/Version.h>
#include <Platform/Logger.h>
#include <glad/glad.h>
#include <C_Utils/PointerList.h>

#include "../CrossRenderer.h"

typedef struct
	{
	GLint MaxTextureUnits;
	struct version OpenGLVersion, GLSLVersion;
	bool DirectStateAccessEnabled, DirectStateAccessAvailable;
	bool SeamlessCubeMapEnabled, SeamlessCubeMapAvailable;
	char **Extensions;
	unsigned ExtensionCount;
	PointerList Framebuffers;
	PointerList Textures;
	PointerList ShaderBuffers;
	PointerList Shaders;
	} OpenGLInformationStruct;

extern OpenGLInformationStruct crGL4Information;

typedef struct
	{
	uvec2 Dimensions;
	GLuint OpenGLID;
	GLint GLMinFilter, GLMagFilter, GLSWrap, GLTWrap;
	GLenum GLTextureType;
	bool Mipmapped;
	crPixelFormat Format;
	} crGL4InternalTextureInfo;

typedef struct
	{
	GLuint OpenGLID;
	GLenum GLAccessType;
	GLenum GLBufferType;
	GLenum GLMappedAccessType;
	void *MappedPointer;
	size_t DataSize, Capacity;
	} crGL4InternalShaderBufferInfo;

typedef struct
	{
	uvec2 Dimensions;
	GLuint OpenGLID;
	crTextureHandle *ColorTextures;
	unsigned ColorTextureCount;
	crTextureHandle DepthTexture;
	crTextureHandle StencilTexture;
	vec4 ClearColor;
	float ClearDepth;
	} crGL4InternalFramebufferInfo;

typedef struct
	{
	GLint OpenGLID;
	crShaderUniformValue CurrentValue;
	crShaderUniformType Type;
	char *Name;
	} crGL4InternalUniformInfo;

typedef struct
	{
	GLint OpenGLID;
	bool Enabled;
	crShaderAttributeType Type;
	char *Name;
	} crGL4InternalAttributeInfo;

typedef struct
	{
	GLuint OpenGLID;
	crGL4InternalUniformInfo *Uniforms;
	unsigned UniformCount;
	crGL4InternalUniformInfo *UniformBlocks;
	unsigned UniformBlockCount;
	crGL4InternalAttributeInfo *Attributes;
	unsigned AttributeCount;

	crShaderInformation GeneralInformation;
	} crGL4InternalShaderInfo;

void OpenGLMessageCallback ( GLenum Source, GLenum Type, GLuint ID, GLenum Severity, GLsizei Length, GLchar const *Message, void const *UserParam );
const char *crGL4StringifyOpenGL ( GLenum Value );
bool crGL4CheckError ( void );

GLenum crGL4TranslateDepthTestMode ( const crDepthTestMode Input );
GLenum crGL4TranslateBlendMode ( const crBlendMode Input );
GLenum crGL4TranslateStencilFunction ( const crStencilFunction Input );
GLenum crGL4TranslateStencilFailAction ( const crStencilFailAction Input );
GLenum crGL4TranslateCullingMode ( const crCullingMode Input );
GLenum crGL4TranslateCullingFaceWinding ( const crCullingFaceWinding Input );
GLenum crGL4TranslatePrimitive ( const crPrimitiveType Input );
GLenum crGL4TranslateTextureFilter ( const crTextureFilter Input );
GLenum crGL4TranslateTextureWrapMode ( const crTextureWrapMode Input );
GLenum crGL4TranslateTextureType ( const crTextureType Input );
GLenum crGL4TranslateShaderBufferComponentType ( const crShaderBufferComponentType Input );
GLenum crGL4TranslateShaderBufferAccessType ( const crShaderBufferAccessType Input );
GLenum crGL4TranslateShaderBufferMapAccessType ( const crShaderBufferMapAccessType Input );
GLenum crGL4TranslateShaderBufferType ( const crShaderBufferType Input );
GLenum crGL4TranslatePolygonMode ( const crPolygonMode Input );
crShaderUniformType crGL4TranslateOpenGLUniformType ( const GLenum Input );
