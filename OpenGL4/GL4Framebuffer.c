#include "GL4Framebuffer.h"
#include "GL4Texture.h"
#include "GL4Internals.h"
#include <assert.h>
#include <malloc.h>

crFramebufferHandle crGL4CreateFramebuffer(const crFramebufferDescriptor Descriptor)
	{
	crGL4InternalFramebufferInfo *NewFramebuffer = calloc(1, sizeof(crGL4InternalFramebufferInfo));

	math_vec4_copy(&NewFramebuffer->ClearColor, Descriptor.ClearColor);
	NewFramebuffer->ClearDepth = 1.0f;
	math_uvec2_copy(&NewFramebuffer->Dimensions, Descriptor.Dimensions);
	NewFramebuffer->ColorTextureCount = Descriptor.ColorAttachments;

	assert(NewFramebuffer);
	pointer_list_node *NewNode = NULL;

	if (Descriptor.DepthEnabled)
		{
		crTextureDescriptor NewTextureDescriptor = { 0 };
		math_uvec2_copy(&NewTextureDescriptor.Dimensions, Descriptor.Dimensions);
		NewTextureDescriptor.Format = Descriptor.DepthFormat;
		NewTextureDescriptor.Type = crTextureType_Texture2D;
		NewFramebuffer->DepthTexture = crGL4CreateTexture(NewTextureDescriptor);
		if (!NewFramebuffer->DepthTexture)
			goto OnError;
		}
	NewFramebuffer->ColorTextures = calloc(Descriptor.ColorAttachments, sizeof(crTextureHandle));
	assert(NewFramebuffer->ColorTextures);
	for (unsigned Index = 0; Index < Descriptor.ColorAttachments; ++Index)
		{
		crTextureDescriptor NewTextureDescriptor = { 0 };
		math_uvec2_copy(&NewTextureDescriptor.Dimensions, Descriptor.Dimensions);
		NewTextureDescriptor.Format = Descriptor.ColorAttachmentFormat;
		NewTextureDescriptor.Type = crTextureType_Texture2D;
		NewFramebuffer->ColorTextures[Index] = crGL4CreateTexture(NewTextureDescriptor);
		if (!NewFramebuffer->ColorTextures[Index])
			goto OnError;
		}

	if (crGL4Information.DirectStateAccessEnabled)
		{
		glCreateFramebuffers(1, &NewFramebuffer->OpenGLID);
		if (Descriptor.DepthEnabled)
			{
			crGL4InternalTextureInfo *TextureInfo = (crGL4InternalTextureInfo *)PointerList_GetNodeData(NewFramebuffer->DepthTexture);
			glNamedFramebufferTexture(NewFramebuffer->OpenGLID, GL_DEPTH_ATTACHMENT, TextureInfo->OpenGLID, 0);
			}
		for (unsigned Index = 0; Index < Descriptor.ColorAttachments; ++Index)
			{
			crGL4InternalTextureInfo *TextureInfo = (crGL4InternalTextureInfo *)PointerList_GetNodeData(NewFramebuffer->ColorTextures[Index]);
			glNamedFramebufferTexture(NewFramebuffer->OpenGLID, GL_COLOR_ATTACHMENT0 + Index, TextureInfo->OpenGLID, 0);
			}
		if (crGL4CheckError() == false)
			goto OnError;

		GLenum FramebufferStatus;
		FramebufferStatus = glCheckNamedFramebufferStatus(NewFramebuffer->OpenGLID, GL_DRAW_FRAMEBUFFER);

		if (FramebufferStatus != GL_FRAMEBUFFER_COMPLETE)
			goto OnError;

		if (Descriptor.ColorAttachments != 0)
			{
			GLuint *Attachments = (GLuint *)alloca(Descriptor.ColorAttachments * sizeof(GLuint));
			for (unsigned Index = 0; Index < Descriptor.ColorAttachments; ++Index)
				Attachments[Index] = GL_COLOR_ATTACHMENT0 + Index;
			glNamedFramebufferDrawBuffers(NewFramebuffer->OpenGLID, Descriptor.ColorAttachments, Attachments);
			}
		}
	else
		{
		glGenFramebuffers(1, &NewFramebuffer->OpenGLID);
		glBindFramebuffer(GL_FRAMEBUFFER, NewFramebuffer->OpenGLID);
		if (Descriptor.DepthEnabled)
			{
			crGL4InternalTextureInfo *TextureInfo = (crGL4InternalTextureInfo *)PointerList_GetNodeData(NewFramebuffer->DepthTexture);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, TextureInfo->OpenGLID, 0);
			}

		for (unsigned Index = 0; Index < Descriptor.ColorAttachments; ++Index)
			{
			crGL4InternalTextureInfo *TextureInfo = (crGL4InternalTextureInfo *)PointerList_GetNodeData(NewFramebuffer->ColorTextures[Index]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + Index, GL_TEXTURE_2D, TextureInfo->OpenGLID, 0);
			}
		if (crGL4CheckError() == false)
			goto OnError;

		GLenum FramebufferStatus;
		FramebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (FramebufferStatus != GL_FRAMEBUFFER_COMPLETE)
			goto OnError;

		if (Descriptor.ColorAttachments != 0)
			{
			GLuint *Attachments = (GLuint *)alloca(Descriptor.ColorAttachments * sizeof(GLuint));
			for (unsigned Index = 0; Index < Descriptor.ColorAttachments; ++Index)
				Attachments[Index] = GL_COLOR_ATTACHMENT0 + Index;
			glDrawBuffers(Descriptor.ColorAttachments, Attachments);
			}
		}

	NewNode = PointerList_AddAtEnd(&crGL4Information.Framebuffers, NewFramebuffer);
	return NewNode;

OnError:
	if (crGL4Information.DirectStateAccessEnabled == false)
		{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	//CurrentBoundFramebuffer = FramebufferHandle::Invalid;

	if (NewFramebuffer->DepthTexture)
		crGL4DeleteTexture(NewFramebuffer->DepthTexture);
	for (size_t cont = 0; cont < NewFramebuffer->ColorTextureCount; ++cont)
		crGL4DeleteTexture(NewFramebuffer->ColorTextures[cont]);
	SAFE_DEL_C(NewFramebuffer->ColorTextures);
	glDeleteFramebuffers(1, &NewFramebuffer->OpenGLID);
	SAFE_DEL_C(NewFramebuffer);
	if (NewNode)
		PointerList_DestroyNode(&crGL4Information.Framebuffers, NewNode);
	return FramebufferHandle_Invalid;
	}

bool crGL4DeleteFramebuffer(const crFramebufferHandle Handle)
	{
	crGL4InternalFramebufferInfo *FramebufferInformation = (crGL4InternalFramebufferInfo *)PointerList_GetNodeData(Handle);

	if (crGL4Information.DirectStateAccessEnabled == false)
		{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	if (FramebufferInformation->DepthTexture)
		crGL4DeleteTexture(FramebufferInformation->DepthTexture);
	for (size_t cont = 0; cont < FramebufferInformation->ColorTextureCount; ++cont)
		crGL4DeleteTexture(FramebufferInformation->ColorTextures[cont]);

	SAFE_DEL_C(FramebufferInformation->ColorTextures);
	glDeleteFramebuffers(1, &FramebufferInformation->OpenGLID);
	SAFE_DEL_C(FramebufferInformation);
	PointerList_DestroyNode(&crGL4Information.Framebuffers, Handle);
	return true;
	}

void crGL4SetFramebufferClearColor(const crFramebufferHandle Handle, const vec4 ClearColor)
	{
	crGL4InternalFramebufferInfo *FramebufferInformation = (crGL4InternalFramebufferInfo *)PointerList_GetNodeData(Handle);
	math_vec4_copy(&FramebufferInformation->ClearColor, ClearColor);
	}

void crGL4SetFramebufferClearDepth(const crFramebufferHandle Handle, const float ClearDepth)
	{
	crGL4InternalFramebufferInfo *FramebufferInformation = (crGL4InternalFramebufferInfo *)PointerList_GetNodeData(Handle);
	FramebufferInformation->ClearDepth = ClearDepth;
	}

void crGL4BindDrawFramebuffer(const crFramebufferHandle Handle)
	{
	UNUSED(Handle);
	//StateCache::ConfigureFramebuffer(Handle);
	}
void crGL4ClearFramebuffer(const crFramebufferHandle Handle, const bool ShouldClearColorBuffer, const vec4 Color, const bool ShouldClearDepthBuffer, const float DepthClearValue, const bool ShouldClearStencilBuffer, const int StencilClearValue, const int StencilMask)
	{
	crGL4InternalFramebufferInfo *FramebufferInformation = (crGL4InternalFramebufferInfo *)PointerList_GetNodeData(Handle);
	int BitMask = 0;
	if (((FramebufferInformation->OpenGLID == 0) || (FramebufferInformation->ColorTextureCount)) && (ShouldClearColorBuffer))
		{
		BitMask |= GL_COLOR_BUFFER_BIT;
		glClearColor(Color.x, Color.y, Color.z, Color.w);
		}
	if (((FramebufferInformation->OpenGLID == 0) || (FramebufferInformation->DepthTexture)) && (ShouldClearDepthBuffer))
		{
		BitMask |= GL_DEPTH_BUFFER_BIT;
		glClearDepth(DepthClearValue);
		}
	if (((FramebufferInformation->OpenGLID == 0) || (FramebufferInformation->StencilTexture)) && (ShouldClearStencilBuffer))
		{
		BitMask |= GL_STENCIL_BUFFER_BIT;
		glStencilMask(StencilMask);
		glClearStencil(StencilClearValue);
		}
	if (BitMask)
		{
		/*if (CurrentBoundFramebuffer != Handle)
			{
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferInformation->OpenGLID);
			CurrentBoundFramebuffer = Handle;
			}

		StateCache::SetDefaultViewportSize(FramebufferInformation->Dimensions);
		StateCache::ConfigureScissor(ScissorSettings());
		StateCache::ConfigureViewport(ViewportSettings());
		StateCache::ConfigureStencil(StencilBufferSettings());*/
		glClear(BitMask);
		}
	crGL4CheckError();
	}

void crGL4ClearFramebufferWithDefaultValues(const crFramebufferHandle Handle)
	{
	crGL4InternalFramebufferInfo *FramebufferInformation = (crGL4InternalFramebufferInfo *)PointerList_GetNodeData(Handle);
	crGL4ClearFramebuffer(Handle, true, FramebufferInformation->ClearColor, true, FramebufferInformation->ClearDepth, true, 1, 1);
	}

void crGL4GetFramebufferSize(const crFramebufferHandle Handle, uvec2 *Dimensions)
	{
	crGL4InternalFramebufferInfo *FramebufferInformation = (crGL4InternalFramebufferInfo *)PointerList_GetNodeData(Handle);
	math_uvec2_copy(Dimensions, FramebufferInformation->Dimensions);
	}

crTextureHandle crGL4GetColorBufferFromFramebuffer(const crFramebufferHandle Handle, const size_t Index)
	{
	crGL4InternalFramebufferInfo *FramebufferInformation = (crGL4InternalFramebufferInfo *)PointerList_GetNodeData(Handle);
	return FramebufferInformation->ColorTextures[Index];
	}

crTextureHandle crGL4GetDepthBufferFromFramebuffer(const crFramebufferHandle Handle)
	{
	crGL4InternalFramebufferInfo *FramebufferInformation = (crGL4InternalFramebufferInfo *)PointerList_GetNodeData(Handle);
	return FramebufferInformation->DepthTexture;
	}
