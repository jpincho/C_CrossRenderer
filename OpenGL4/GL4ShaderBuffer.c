#include "GL4ShaderBuffer.h"
#include "GL4Internals.h"
#include <assert.h>

crShaderBufferHandle crGL4CreateShaderBuffer ( const crShaderBufferDescriptor Descriptor )
	{
	crGL4InternalShaderBufferInfo *NewShaderBuffer = calloc ( 1, sizeof ( crGL4InternalShaderBufferInfo ) );
	assert ( NewShaderBuffer != NULL );
	PointerListNode *NewNode = NULL;

	NewShaderBuffer->GLAccessType = crGL4TranslateShaderBufferAccessType ( Descriptor.AccessType );
	NewShaderBuffer->GLBufferType = crGL4TranslateShaderBufferType ( Descriptor.BufferType );
	NewShaderBuffer->Capacity = Descriptor.Capacity;
	NewShaderBuffer->DataSize = Descriptor.DataSize;

	if ( crGL4Information.DirectStateAccessEnabled )
		{
		glCreateBuffers ( 1, &NewShaderBuffer->OpenGLID );
		if ( crGL4CheckError() == false )
			goto OnError;
		if ( ( Descriptor.Data ) && ( Descriptor.DataSize ) )
			glNamedBufferData ( NewShaderBuffer->OpenGLID, NewShaderBuffer->DataSize, Descriptor.Data, NewShaderBuffer->GLAccessType );
		else
			glNamedBufferData ( NewShaderBuffer->OpenGLID, NewShaderBuffer->Capacity, NULL, NewShaderBuffer->GLAccessType );
		if ( crGL4CheckError() == false )
			goto OnError;
		}
	else
		{
		glGenBuffers ( 1, &NewShaderBuffer->OpenGLID );
		if ( crGL4CheckError() == false )
			goto OnError;

		glBindBuffer ( NewShaderBuffer->GLBufferType, NewShaderBuffer->OpenGLID );
		if ( ( Descriptor.Data ) && ( Descriptor.DataSize ) )
			glBufferData ( NewShaderBuffer->GLBufferType, NewShaderBuffer->DataSize, Descriptor.Data, NewShaderBuffer->GLAccessType );
		else
			glBufferData ( NewShaderBuffer->GLBufferType, NewShaderBuffer->Capacity, NULL, NewShaderBuffer->GLAccessType );

		if ( crGL4CheckError() == false )
			goto OnError;
		}

	NewNode = PointerList_AddAtEnd ( &crGL4Information.ShaderBuffers, NewShaderBuffer );
	return NewNode;

OnError:
	if ( NewShaderBuffer->OpenGLID )
		glDeleteBuffers ( 1, &NewShaderBuffer->OpenGLID );
	SAFE_DEL_C ( NewShaderBuffer );
	return crShaderBufferHandle_Invalid;
	}

bool crGL4DeleteShaderBuffer ( const crShaderBufferHandle Handle )
	{
	crGL4InternalShaderBufferInfo *ShaderBufferInformation = ( crGL4InternalShaderBufferInfo * ) PointerList_GetNodeData ( Handle );

	glDeleteBuffers ( 1, &ShaderBufferInformation->OpenGLID );
	free ( ShaderBufferInformation );
	PointerList_DestroyNode ( &crGL4Information.ShaderBuffers, Handle );
	return true;
	}

bool crGL4ChangeShaderBufferContents ( const crShaderBufferHandle Handle, const size_t Offset, const void *Data, const size_t DataSize )
	{
	crGL4InternalShaderBufferInfo *ShaderBufferInformation = ( crGL4InternalShaderBufferInfo * ) PointerList_GetNodeData ( Handle );

	if ( Offset + DataSize > ShaderBufferInformation->Capacity )
		return false;

	if ( crGL4Information.DirectStateAccessEnabled )
		{
		glNamedBufferSubData ( ShaderBufferInformation->OpenGLID, Offset, DataSize, Data );
		}
	else
		{
		glBindBuffer ( ShaderBufferInformation->GLBufferType, ShaderBufferInformation->OpenGLID );
		glBufferSubData ( ShaderBufferInformation->GLBufferType, Offset, DataSize, Data );

		glBindBuffer ( ShaderBufferInformation->GLBufferType, 0 ); // Unbind it
		}
	return crGL4CheckError();
	}

void *crGL4MapShaderBuffer ( const crShaderBufferHandle Handle, const crShaderBufferMapAccessType AccessType )
	{
	crGL4InternalShaderBufferInfo *ShaderBufferInformation = ( crGL4InternalShaderBufferInfo * ) PointerList_GetNodeData ( Handle );
	GLenum GLAccessType = crGL4TranslateShaderBufferMapAccessType ( AccessType );

	if ( ( ShaderBufferInformation->MappedPointer ) && ( ShaderBufferInformation->GLMappedAccessType == GLAccessType ) )
		return ShaderBufferInformation->MappedPointer;
	if ( ShaderBufferInformation->MappedPointer != NULL )
		{
		if ( crGL4UnmapShaderBuffer ( Handle ) == false )
			return NULL;
		}

	if ( crGL4Information.DirectStateAccessEnabled )
		{
		ShaderBufferInformation->MappedPointer = glMapNamedBuffer ( ShaderBufferInformation->OpenGLID, GLAccessType );
		ShaderBufferInformation->GLMappedAccessType = GLAccessType;
		if ( crGL4CheckError() == false )
			return NULL;
		}
	else
		{
		glBindBuffer ( ShaderBufferInformation->GLBufferType, ShaderBufferInformation->OpenGLID );
		if ( crGL4CheckError() == false )
			return NULL;
		ShaderBufferInformation->MappedPointer = glMapBuffer ( ShaderBufferInformation->GLBufferType, GLAccessType );
		ShaderBufferInformation->GLMappedAccessType = GLAccessType;
		if ( crGL4CheckError() == false )
			return NULL;
		}
	return ShaderBufferInformation->MappedPointer;
	}

bool crGL4UnmapShaderBuffer ( const crShaderBufferHandle Handle )
	{
	crGL4InternalShaderBufferInfo *ShaderBufferInformation = ( crGL4InternalShaderBufferInfo * ) PointerList_GetNodeData ( Handle );

	if ( ShaderBufferInformation->MappedPointer == NULL )
		return true;

	if ( crGL4Information.DirectStateAccessEnabled )
		{
		glUnmapNamedBuffer ( ShaderBufferInformation->OpenGLID );
		}
	else
		{
		glBindBuffer ( ShaderBufferInformation->GLBufferType, ShaderBufferInformation->OpenGLID );
		glUnmapBuffer ( ShaderBufferInformation->GLBufferType );
		}
	ShaderBufferInformation->MappedPointer = NULL;
	return crGL4CheckError();
	}
