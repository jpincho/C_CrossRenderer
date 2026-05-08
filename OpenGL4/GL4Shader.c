#include "GL4Shader.h"
#include "GL4Internals.h"
#include <string.h>
#include <assert.h>
#include <malloc.h>
#include <C_Utils/ArrayUtils.h>

#if defined (PLATFORM_COMPILER_MSVC)
#define strdup _strdup
#endif

static bool ReallocAndClear ( void **Pointer, const unsigned CurrentSize, const unsigned NewSize )
	{
	if ( CurrentSize == NewSize )
		return true;

	void *NewPointer = realloc ( *Pointer, NewSize );
	if ( NewPointer == NULL )
		return false;
	*Pointer = NewPointer;
	// Clears the new area
	if ( NewSize > CurrentSize )
		memset ( ( char * ) * Pointer + CurrentSize, 0, NewSize - CurrentSize );
	return true;
	}

static bool BuildShaderObject ( const char *Code, const GLenum GLShaderType, GLuint *OutputID )
	{
	GLuint ShaderObjectID = glCreateShader ( GLShaderType );

	int CompileStatus = 0;
	const char *Pointers[1];
	Pointers[0] = Code;
	glShaderSource ( ShaderObjectID, ( GLsizei ) 1, Pointers, NULL );
	glCompileShader ( ShaderObjectID );
	glGetShaderiv ( ShaderObjectID, GL_COMPILE_STATUS, &CompileStatus );

	if ( ( crGL4CheckError() == false ) || ( CompileStatus != GL_TRUE ) )
		{
		GLchar *Infolog = NULL;
		int InfoLogLength;
		glGetShaderiv ( ShaderObjectID, GL_INFO_LOG_LENGTH, &InfoLogLength );
		if ( InfoLogLength )
			{
			Infolog = alloca ( InfoLogLength );
			glGetShaderInfoLog ( ShaderObjectID, InfoLogLength, NULL, Infolog );
			}
		LOG_ERROR ( "OpenGL error during shader object compilation. '%s'", Infolog );
		return false;
		}
	*OutputID = ShaderObjectID;
	return true;
	}

static void DestroycrGL4InternalShaderInfo ( crGL4InternalShaderInfo *ShaderInfo )
	{
	for ( unsigned AttributeIndex = 0; AttributeIndex < ShaderInfo->AttributeCount; ++AttributeIndex )
		{
		SAFE_DEL_C ( ShaderInfo->Attributes[AttributeIndex].Name );
		}
	SAFE_DEL_C ( ShaderInfo->Attributes );
	for ( unsigned UniformIndex = 0; UniformIndex < ShaderInfo->UniformCount; ++UniformIndex )
		{
		SAFE_DEL_C ( ShaderInfo->Uniforms[UniformIndex].Name );
		}
	SAFE_DEL_C ( ShaderInfo->Uniforms );
	}

static bool DetectUniformsAndAttributes ( crGL4InternalShaderInfo *ShaderInfo )
	{
	GLint UniformCount, MaxUniformNameLength, UniformBlockCount;
	GLint AttributeCount, MaxAttributeNameLength;

	glGetProgramiv ( ShaderInfo->OpenGLID, GL_ACTIVE_UNIFORMS, &UniformCount );
	glGetProgramiv ( ShaderInfo->OpenGLID, GL_ACTIVE_ATTRIBUTES, &AttributeCount );
	glGetProgramiv ( ShaderInfo->OpenGLID, GL_ACTIVE_UNIFORM_BLOCKS, &UniformBlockCount );

	if ( !crGL4CheckError() )
		return false;
	if ( ( UniformCount == 0 ) && ( AttributeCount == 0 ) && ( UniformBlockCount == 0 ) )
		return true;

	glGetProgramiv ( ShaderInfo->OpenGLID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &MaxUniformNameLength );
	glGetProgramiv ( ShaderInfo->OpenGLID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &MaxAttributeNameLength );
	if ( !crGL4CheckError() )
		return false;

	size_t NameLength = MAX ( MaxUniformNameLength, MaxAttributeNameLength );
	char *Name = alloca ( NameLength + 1 );
	if ( Name == NULL )
		return false;


	unsigned NeededUniformCapacity = UniformBlockCount + UniformCount;
	ShaderInfo->UniformCount = 0;
	if ( UniformBlockCount != 0 )
		{
		ShaderInfo->Uniforms = calloc ( NeededUniformCapacity, sizeof ( crGL4InternalUniformInfo ) );
		if ( ShaderInfo->Uniforms == NULL )
			goto OnError;
		}

	for ( int UniformBlockIndex = 0; UniformBlockIndex < UniformBlockCount; ++UniformBlockIndex )
		{
		GLint Location;

		glGetActiveUniformBlockName ( ShaderInfo->OpenGLID, UniformBlockIndex, ( GLsizei ) NameLength, NULL, Name );
		if ( !crGL4CheckError() )
			goto OnError;
		if ( strncmp ( "gl_", Name, 3 ) == 0 )
			continue;

		Location = glGetUniformBlockIndex ( ShaderInfo->OpenGLID, Name );
		if ( !crGL4CheckError() )
			goto OnError;
		if ( Location == -1 )
			{
			LOG_ERROR ( "Unable to get location for uniform block '%s'", Name );
			goto OnError;
			}

		ShaderInfo->Uniforms[ShaderInfo->UniformCount].Name = strdup ( Name );
		ShaderInfo->Uniforms[ShaderInfo->UniformCount].OpenGLID = Location;
		ShaderInfo->Uniforms[ShaderInfo->UniformCount].Type = crShaderUniformType_Block;
		++ShaderInfo->UniformCount;
		}

	unsigned FinalUniformBlockCount = ShaderInfo->UniformCount;
	for ( int UniformIndex = 0; UniformIndex < UniformCount; ++UniformIndex )
		{
		GLenum GLType;
		GLint Location;
		GLint UniformSize;

		glGetActiveUniform ( ShaderInfo->OpenGLID, UniformIndex, ( GLsizei ) NameLength, NULL, &UniformSize, &GLType, Name );
		if ( !crGL4CheckError() )
			goto OnError;
		if ( strncmp ( "gl_", Name, 3 ) == 0 )
			continue;

		Location = glGetUniformLocation ( ShaderInfo->OpenGLID, Name );
		if ( !crGL4CheckError() )
			goto OnError;
		// If it returns -1, it could be a uniform inside a uniform block. Check if its name begins with a uniform block's name
		if ( Location == -1 )
			{
			bool IsPartOfBlock = false;
			for ( unsigned BlockIterator = 0; BlockIterator < FinalUniformBlockCount; ++BlockIterator )
				{
				if ( strncmp ( Name, ShaderInfo->Uniforms[BlockIterator].Name, strlen ( ShaderInfo->Uniforms[BlockIterator].Name ) ) == 0 )
					{
					IsPartOfBlock = true;
					break;
					}
				}
			if ( IsPartOfBlock )
				continue;// It's a uniform inside of a block. for some fucking reason, I can't get its location...
			LOG_ERROR ( "Unable to get location for uniform '%s'", Name );
			continue;
			}

		// If this is uniform is an array, add all other elements to the list
		if ( UniformSize != 1 )
			{
			// First extend our array...
			NeededUniformCapacity += UniformSize - 1;
			if ( ReallocAndClear ( ( void ** ) &ShaderInfo->Uniforms, ShaderInfo->UniformCount * sizeof ( crGL4InternalUniformInfo ), NeededUniformCapacity * sizeof ( crGL4InternalUniformInfo ) ) == false )
				goto OnError;

			// Add all the elements to our shader info
			char *BracketPos = strchr ( Name, '[' );
			*BracketPos = 0;
			for ( int Index = 0; Index < UniformSize; ++Index )
				{
				int NecessarySpace = snprintf ( NULL, 0, "%s[%u]", Name, Index );
				ShaderInfo->Uniforms[ShaderInfo->UniformCount].Name = malloc ( NecessarySpace + 1 );
				snprintf ( ShaderInfo->Uniforms[ShaderInfo->UniformCount].Name, NecessarySpace + 1, "%s[%u]", Name, Index );
				ShaderInfo->Uniforms[ShaderInfo->UniformCount].Type = crGL4TranslateOpenGLUniformType ( GLType );
				ShaderInfo->Uniforms[ShaderInfo->UniformCount].OpenGLID = Location + Index;
				++ShaderInfo->UniformCount;
				++UniformIndex;
				}
			}
		else
			{
			ShaderInfo->Uniforms[ShaderInfo->UniformCount].Name = strdup ( Name );
			ShaderInfo->Uniforms[ShaderInfo->UniformCount].OpenGLID = Location;
			ShaderInfo->Uniforms[ShaderInfo->UniformCount].Type = crGL4TranslateOpenGLUniformType ( GLType );
			++ShaderInfo->UniformCount;
			}
		}

	// Some uniforms may have been skipped due to being part of a block, so extra memory has been allocated. adjust here
	if ( ShaderInfo->UniformCount != NeededUniformCapacity )
		{
		if ( ReallocAndClear ( ( void ** ) &ShaderInfo->Uniforms, ShaderInfo->UniformCount * sizeof ( crGL4InternalUniformInfo ), NeededUniformCapacity * sizeof ( crGL4InternalUniformInfo ) ) == false )
			goto OnError;
		}

	ShaderInfo->AttributeCount = 0;
	if ( AttributeCount != 0 )
		{
		ShaderInfo->Attributes = calloc ( AttributeCount, sizeof ( crGL4InternalAttributeInfo ) );
		if ( ShaderInfo->Attributes == NULL )
			goto OnError;
		}
	for ( int AttributeIndex = 0; AttributeIndex < AttributeCount; ++AttributeIndex )
		{
		GLenum GLType;
		GLint Location;
		GLint AttributeSize;

		glGetActiveAttrib ( ShaderInfo->OpenGLID, AttributeIndex, ( GLsizei ) NameLength, NULL, &AttributeSize, &GLType, Name );
		if ( !crGL4CheckError() )
			goto OnError;
		if ( strncmp ( "gl_", Name, 3 ) == 0 )
			continue;

		Location = glGetAttribLocation ( ShaderInfo->OpenGLID, Name );
		if ( !crGL4CheckError() )
			goto OnError;
		if ( Location == -1 )
			{
			LOG_ERROR ( "Unable to get location for attribute '%s'", Name );
			goto OnError;
			}

		ShaderInfo->Attributes[ShaderInfo->AttributeCount].Name = strdup ( Name );
		ShaderInfo->Attributes[ShaderInfo->AttributeCount].OpenGLID = Location;
		ShaderInfo->Attributes[ShaderInfo->AttributeCount].Type = ( crShaderAttributeType ) crGL4TranslateOpenGLUniformType ( GLType );
		ShaderInfo->Attributes[ShaderInfo->AttributeCount].Enabled = false;
		++ShaderInfo->AttributeCount;
		}
	return true;

OnError:
	DestroycrGL4InternalShaderInfo ( ShaderInfo );
	return false;
	}

crShaderHandle crGL4CreateShader ( crShaderCode NewCode )
	{
	crGL4InternalShaderInfo *NewShader = calloc ( 1, sizeof ( crGL4InternalShaderInfo ) );
	assert ( NewShader );
	PointerListNode *NewNode = NULL;

	NewShader->OpenGLID = glCreateProgram();
	if ( crGL4CheckError() == false )
		return crShaderHandle_Invalid;

	GLuint VertexShaderID = ( GLuint ) - 1, FragmentShaderID = ( GLuint ) - 1, GeometryShaderID = ( GLuint ) - 1;
	bool VertexShaderBuilt = false, FragmentShaderBuilt = false, GeometryShaderBuilt = false;
	if ( NewCode.VertexShader )
		{
		if ( BuildShaderObject ( NewCode.VertexShader, GL_VERTEX_SHADER, &VertexShaderID ) == false )
			goto OnError;
		VertexShaderBuilt = true;
		glAttachShader ( NewShader->OpenGLID, VertexShaderID );
		}

	if ( NewCode.FragmentShader )
		{
		if ( BuildShaderObject ( NewCode.FragmentShader, GL_FRAGMENT_SHADER, &FragmentShaderID ) == false )
			goto OnError;
		FragmentShaderBuilt = true;
		glAttachShader ( NewShader->OpenGLID, FragmentShaderID );
		}

	if ( NewCode.GeometryShader )
		{
		if ( BuildShaderObject ( NewCode.GeometryShader, GL_GEOMETRY_SHADER, &GeometryShaderID ) == false )
			goto OnError;
		GeometryShaderBuilt = true;
		glAttachShader ( NewShader->OpenGLID, GeometryShaderID );
		}

	glLinkProgram ( NewShader->OpenGLID );

	GLint LinkStatus;
	glGetProgramiv ( NewShader->OpenGLID, GL_LINK_STATUS, &LinkStatus );
	if ( LinkStatus != GL_TRUE )
		{
		GLchar *Infolog = NULL;
		int InfoLogLength;
		glGetProgramiv ( NewShader->OpenGLID, GL_INFO_LOG_LENGTH, &InfoLogLength );
		if ( InfoLogLength )
			{
			Infolog = alloca ( InfoLogLength );
			glGetProgramInfoLog ( NewShader->OpenGLID, InfoLogLength, NULL, Infolog );
			}

		LOG_ERROR ( "OpenGL error during shader program linking. '%s'", Infolog );
		goto OnError;
		}
	LOG_DEBUG ( "Linked shader program %u", NewShader->OpenGLID );

	if ( VertexShaderBuilt )
		glDeleteShader ( VertexShaderID );
	if ( FragmentShaderBuilt )
		glDeleteShader ( FragmentShaderID );
	if ( GeometryShaderBuilt )
		glDeleteShader ( GeometryShaderID );

	if ( DetectUniformsAndAttributes ( NewShader ) == false )
		goto OnError;

	// Populate shader information structure
	NewShader->GeneralInformation.AttributeCount = NewShader->AttributeCount;
	NewShader->GeneralInformation.Attributes = calloc ( NewShader->AttributeCount, sizeof ( crShaderAttributeInformation ) );
	for ( unsigned Index = 0; Index < NewShader->AttributeCount; ++Index )
		{
		NewShader->GeneralInformation.Attributes[Index].Handle = Index;
		NewShader->GeneralInformation.Attributes[Index].Name = NewShader->Attributes[Index].Name;
		NewShader->GeneralInformation.Attributes[Index].Type = NewShader->Attributes[Index].Type;
		}
	NewShader->GeneralInformation.UniformCount = NewShader->UniformCount;
	NewShader->GeneralInformation.Uniforms = calloc ( NewShader->UniformCount, sizeof ( crShaderUniformInformation ) );
	for ( unsigned Index = 0; Index < NewShader->UniformCount; ++Index )
		{
		NewShader->GeneralInformation.Uniforms[Index].Handle = Index;
		NewShader->GeneralInformation.Uniforms[Index].Name = NewShader->Uniforms[Index].Name;
		NewShader->GeneralInformation.Uniforms[Index].Type = NewShader->Uniforms[Index].Type;
		}

	NewNode = PointerList_AddAtEnd ( &crGL4Information.Shaders, NewShader );
	return NewNode;

OnError:
	if ( NewShader->OpenGLID )
		glDeleteProgram ( NewShader->OpenGLID );
	if ( VertexShaderBuilt )
		glDeleteShader ( VertexShaderID );
	if ( FragmentShaderBuilt )
		glDeleteShader ( FragmentShaderID );
	if ( GeometryShaderBuilt )
		glDeleteShader ( GeometryShaderID );
	free ( NewShader );
	return crShaderHandle_Invalid;
	}

bool crGL4DeleteShader ( const crShaderHandle Handle )
	{
	crGL4InternalShaderInfo *ShaderInformation = ( crGL4InternalShaderInfo * ) PointerList_GetNodeData ( Handle );

	glDeleteProgram ( ShaderInformation->OpenGLID );
	for ( unsigned Index = 0; Index < ShaderInformation->AttributeCount; ++Index )
		{
		SAFE_DEL_C ( ShaderInformation->Attributes[Index].Name );
		}
	SAFE_DEL_C ( ShaderInformation->Attributes );

	for ( unsigned Index = 0; Index < ShaderInformation->UniformCount; ++Index )
		{
		SAFE_DEL_C ( ShaderInformation->Uniforms[Index].Name );
		}
	SAFE_DEL_C ( ShaderInformation->Uniforms );
	SAFE_DEL_C ( ShaderInformation );
	PointerList_DestroyNode ( &crGL4Information.Shaders, Handle );
	return true;
	}

const crShaderInformation *crGL4GetShaderInformation ( const crShaderHandle Handle )
	{
	crGL4InternalShaderInfo *ShaderInformation = ( crGL4InternalShaderInfo * ) PointerList_GetNodeData ( Handle );
	return &ShaderInformation->GeneralInformation;
	}
