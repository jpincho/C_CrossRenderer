#include "GL4StateCache.h"
#include "GL4Internals.h"

static bool Enabled = true;
static uvec2 DefaultViewportSize;
static crRenderState CurrentState;
static crFramebufferHandle CurrentBoundFramebuffer = 0;

void crGL4SetDefaultViewportSize(uvec2 NewSize)
	{
	math_uvec2_copy(&DefaultViewportSize, NewSize);
	}

bool crGL4ApplyState(const crRenderState *NewState)
	{
	if (Enabled)
		{
		return crGL4ApplyStateWithCache(NewState);
		}
	else
		{
		return crGL4ApplyStateWithoutCache(NewState);
		}
	}

bool crGL4ApplyStateWithCache(const crRenderState *NewState)
	{
	crGL4ConfigureCulling(&NewState->Culling);
	crGL4ConfigureBlending(&NewState->Blending);
	crGL4ConfigureStencil(&NewState->Stencil);
	crGL4ConfigureScissor(&NewState->Scissor);
	crGL4ConfigureViewport(&NewState->Viewport);
	crGL4ConfigureDepthTest(&NewState->DepthTest);
	crGL4ConfigurePolygonMode(&NewState->PolygonMode);
	return crGL4CheckError();
	}

bool crGL4ApplyStateWithoutCache(const crRenderState *NewState)
	{
	CurrentState = *NewState;
	if (CurrentState.Culling.Enabled)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	glCullFace(crGL4TranslateCullingMode(CurrentState.Culling.Mode));
	glFrontFace(crGL4TranslateCullingFaceWinding(CurrentState.Culling.Winding));

	if (CurrentState.Blending.Enabled)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
	glBlendFunc(crGL4TranslateBlendMode(CurrentState.Blending.Source), crGL4TranslateBlendMode(CurrentState.Blending.Destination));

	if (CurrentState.Stencil.Enabled)
		glEnable(GL_STENCIL_TEST);
	else
		glDisable(GL_STENCIL_TEST);
	glStencilMask(CurrentState.Stencil.Mask);
	glStencilFunc(crGL4TranslateStencilFunction(CurrentState.Stencil.Function), CurrentState.Stencil.FunctionReference, CurrentState.Stencil.FunctionMask);
	glStencilOp(crGL4TranslateStencilFailAction(CurrentState.Stencil.OnFail), crGL4TranslateStencilFailAction(CurrentState.Stencil.OnFailZ), crGL4TranslateStencilFailAction(CurrentState.Stencil.OnPassZ));

	if (CurrentState.Scissor.Enabled)
		glEnable(GL_SCISSOR_TEST);
	else
		glDisable(GL_SCISSOR_TEST);
	glScissor(CurrentState.Scissor.LowerLeft.x, CurrentState.Scissor.LowerLeft.y, CurrentState.Scissor.Dimensions.x, CurrentState.Scissor.Dimensions.y);

	if (CurrentState.Viewport.Enabled == false)
		glViewport(0, 0, DefaultViewportSize.x, DefaultViewportSize.y);
	else
		glViewport(CurrentState.Viewport.LowerLeft.x, CurrentState.Viewport.LowerLeft.y, CurrentState.Viewport.Dimensions.x, CurrentState.Viewport.Dimensions.y);

	if (CurrentState.DepthTest.Enabled)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	glDepthFunc(crGL4TranslateDepthTestMode(CurrentState.DepthTest.Mode));

	glPolygonMode(GL_FRONT_AND_BACK, crGL4TranslatePolygonMode(CurrentState.PolygonMode.State));
	return crGL4CheckError();
	}

void crGL4Invalidate(void)
	{
	SetRenderStateToDefault(&CurrentState);
	CurrentState.Viewport.Dimensions = DefaultViewportSize;
	crGL4ApplyStateWithoutCache(&CurrentState);
	}

crRenderState crGL4GetCurrentState(void)
	{
	return CurrentState;
	}

void crGL4ConfigureCulling(const crCullingSettings *NewSettings)
	{
	if (NewSettings->Enabled != CurrentState.Culling.Enabled)
		{
		if (NewSettings->Enabled)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);

		CurrentState.Culling.Enabled = NewSettings->Enabled;
		}
	if (CurrentState.Culling.Enabled == false)
		return;
	if (NewSettings->Mode != CurrentState.Culling.Mode)
		{
		glCullFace(crGL4TranslateCullingMode(NewSettings->Mode));
		CurrentState.Culling.Mode = NewSettings->Mode;
		}
	if (NewSettings->Winding != CurrentState.Culling.Winding)
		{
		glFrontFace(crGL4TranslateCullingFaceWinding(NewSettings->Winding));
		CurrentState.Culling.Winding = NewSettings->Winding;
		}
	crGL4CheckError();
	}

void crGL4ConfigureBlending(const crBlendSettings *NewSettings)
	{
	// This is basically disabling blending...
	if (((NewSettings->Source == crBlendMode_One) && (NewSettings->Destination == crBlendMode_Zero)) || (NewSettings->Enabled == false))
		{
		glDisable(GL_BLEND);
		CurrentState.Blending.Enabled = false;
		return;
		}

	if (NewSettings->Enabled != CurrentState.Blending.Enabled)
		{
		if (NewSettings->Enabled)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
		CurrentState.Blending.Enabled = NewSettings->Enabled;
		}
	if (CurrentState.Blending.Enabled == false)
		return;
	if ((NewSettings->Source != CurrentState.Blending.Source) || (NewSettings->Destination != CurrentState.Blending.Destination))
		{
		glBlendFunc(crGL4TranslateBlendMode(NewSettings->Source), crGL4TranslateBlendMode(NewSettings->Destination));
		CurrentState.Blending.Source = NewSettings->Source;
		CurrentState.Blending.Destination = NewSettings->Destination;
		}

	crGL4CheckError();
	}

void crGL4ConfigureStencil(const crStencilBufferSettings *NewSettings)
	{
	// If there's a change in state...
	if (NewSettings->Enabled != CurrentState.Stencil.Enabled)
		{
		if (NewSettings->Enabled)
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);
		CurrentState.Stencil.Enabled = NewSettings->Enabled;
		}

	if (CurrentState.Stencil.Enabled == false)
		return;

	if (NewSettings->Mask != CurrentState.Stencil.Mask)
		{
		glStencilMask(NewSettings->FunctionMask);
		CurrentState.Stencil.Mask = NewSettings->Mask;
		}

	if ((NewSettings->FunctionReference != CurrentState.Stencil.FunctionReference) ||
	        (NewSettings->FunctionMask != CurrentState.Stencil.FunctionMask) ||
	        (NewSettings->Function != CurrentState.Stencil.Function))
		{
		glStencilFunc(crGL4TranslateStencilFunction(NewSettings->Function), NewSettings->FunctionReference, NewSettings->FunctionMask);
		CurrentState.Stencil.Function = NewSettings->Function;
		CurrentState.Stencil.FunctionMask = NewSettings->FunctionMask;
		CurrentState.Stencil.FunctionReference = NewSettings->FunctionReference;
		}

	if ((NewSettings->OnFail != CurrentState.Stencil.OnFail) ||
	        (NewSettings->OnFailZ != CurrentState.Stencil.OnFailZ) ||
	        (NewSettings->OnPassZ != CurrentState.Stencil.OnPassZ))
		{
		glStencilOp(crGL4TranslateStencilFailAction(NewSettings->OnFail), crGL4TranslateStencilFailAction(NewSettings->OnFailZ), crGL4TranslateStencilFailAction(NewSettings->OnPassZ));
		CurrentState.Stencil.OnFail = NewSettings->OnFail;
		CurrentState.Stencil.OnFailZ = NewSettings->OnFailZ;
		CurrentState.Stencil.OnPassZ = NewSettings->OnPassZ;
		}

	crGL4CheckError();
	}

void crGL4ConfigureScissor(const crScissorSettings *NewSettings)
	{
	if (CurrentState.Scissor.Enabled != NewSettings->Enabled)
		{
		if (NewSettings->Enabled)
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);
		CurrentState.Scissor.Enabled = NewSettings->Enabled;
		}
	if (CurrentState.Scissor.Enabled == false)
		return;

	if ((math_uvec2_equals(CurrentState.Scissor.LowerLeft, NewSettings->LowerLeft) == false) ||
	        (math_uvec2_equals(CurrentState.Scissor.Dimensions, NewSettings->Dimensions) == false))
		{
		glScissor(NewSettings->LowerLeft.x, NewSettings->LowerLeft.y, NewSettings->Dimensions.x, NewSettings->Dimensions.y);
		math_uvec2_copy(&CurrentState.Scissor.Dimensions, NewSettings->Dimensions);
		math_uvec2_copy(&CurrentState.Scissor.LowerLeft, NewSettings->LowerLeft);
		}

	crGL4CheckError();
	}

void crGL4ConfigureViewport(const crViewportSettings *NewSettings)
	{
	// Keep it disabled? early out
	if ((NewSettings->Enabled == CurrentState.Viewport.Enabled) && (NewSettings->Enabled == false))
		return;
	// Keep it enabled with the same size? early out
	if ((NewSettings->Enabled == CurrentState.Viewport.Enabled) && (NewSettings->Enabled == true) &&
	        (math_uvec2_equals(NewSettings->Dimensions, CurrentState.Viewport.Dimensions) == true) &&
	        (math_uvec2_equals(NewSettings->LowerLeft, CurrentState.Viewport.LowerLeft) == true))
		return;

	if (NewSettings->Enabled == false)
		{
		glViewport(0, 0, DefaultViewportSize.x, DefaultViewportSize.y);
		CurrentState.Viewport.LowerLeft.x = 0;
		CurrentState.Viewport.LowerLeft.y = 0;
		CurrentState.Viewport.Dimensions.x = DefaultViewportSize.x;
		CurrentState.Viewport.Dimensions.y = DefaultViewportSize.y;
		CurrentState.Viewport.Enabled = false;
		return;
		}

	if ((math_uvec2_equals(NewSettings->Dimensions, CurrentState.Viewport.Dimensions) == false) ||
	        (math_uvec2_equals(NewSettings->LowerLeft, CurrentState.Viewport.LowerLeft) == false))
		{
		glViewport(NewSettings->LowerLeft.x, NewSettings->LowerLeft.y, NewSettings->Dimensions.x, NewSettings->Dimensions.y);
		math_uvec2_copy(&CurrentState.Viewport.Dimensions, NewSettings->Dimensions);
		math_uvec2_copy(&CurrentState.Viewport.LowerLeft, NewSettings->LowerLeft);
		CurrentState.Viewport.Enabled = true;
		}
	crGL4CheckError();
	}

void crGL4ConfigureDepthTest(const crDepthTestSettings *NewSettings)
	{
	if (CurrentState.DepthTest.Enabled != NewSettings->Enabled)
		{
		if (NewSettings->Enabled)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
		CurrentState.DepthTest.Enabled = NewSettings->Enabled;
		}
	if (CurrentState.DepthTest.Enabled == false)
		return;

	if (CurrentState.DepthTest.Mode != NewSettings->Mode)
		{
		glDepthFunc(crGL4TranslateDepthTestMode(NewSettings->Mode));
		CurrentState.DepthTest.Mode = NewSettings->Mode;
		}
	crGL4CheckError();
	}

void crGL4ConfigureFramebuffer(const crFramebufferHandle NewFramebuffer)
	{
	if (NewFramebuffer == CurrentBoundFramebuffer)
		return;
	if (!NewFramebuffer)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	else
		{
		crGL4InternalFramebufferInfo *FramebufferInformation = (crGL4InternalFramebufferInfo *)PointerList_GetNodeData(NewFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferInformation->OpenGLID);
		}
	CurrentBoundFramebuffer = NewFramebuffer;
	crGL4CheckError();
	}

void crGL4ConfigurePolygonMode(const crPolygonModeSettings *NewSettings)
	{
	if (CurrentState.PolygonMode.State != NewSettings->State)
		{
		glPolygonMode(GL_FRONT_AND_BACK, crGL4TranslatePolygonMode(CurrentState.PolygonMode.State));
		CurrentState.PolygonMode.State = NewSettings->State;
		crGL4CheckError();
		}
	}
