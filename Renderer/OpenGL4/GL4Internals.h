#pragma once
#include <glad/glad.h>
#include "../texture_descriptor.h"
#include "../../../C_Common/pointer_list.h"

typedef struct
	{
/*	GLint MaxTextureUnits;
	Version OpenGLVersion, GLSLVersion;
	std::string OpenGLRendererString, OpenGLVendorString;
	std::vector <std::string> Extensions;*/
	bool DirectStateAccessEnabled;
	}OpenGLInformationStruct;

extern OpenGLInformationStruct OpenGLInformation;

typedef struct
	{
	ivec2 Dimensions;
	GLuint OpenGLID;
	GLint GLMinFilter, GLMagFilter, GLSWrap, GLTWrap;
	GLenum GLTextureType;
	bool Mipmapped;
	EPixelFormat Format;
	}GLInternalTextureInfo;

void OpenGLMessageCallback ( GLenum Source, GLenum Type, GLuint ID, GLenum Severity, GLsizei Length, GLchar const *Message, void const *UserParam );
const char *StringifyOpenGL ( GLenum Value );
bool CheckError ( void );

bool TranslateTextureFilter ( const ETextureFilter Input, GLint *Output );
bool TranslateTextureWrapMode ( const ETextureWrapMode Input, GLint *Output );
bool TranslateTextureType ( const ETextureType Input, GLenum *Output );
