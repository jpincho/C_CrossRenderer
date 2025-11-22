#include "GL4Renderer.h"
#include "GL4Internals.h"
#include "GL4StateCache.h"
#include "GL4Context.h"
#include <string.h>

#if defined(_MSC_VER)
#pragma warning(disable:4996)
#endif

unsigned GeneralVAO;
static crShaderHandle ActiveShader = NULL;

static bool DetectOpenGLInformation(void)
	{
	// MAJOR & MINOR only introduced in GL {,ES} 3.0
	GLint TempMajor = 0, TempMinor = 0;
	Version_Set(&crGL4Information.OpenGLVersion, 0, 0);
	glGetIntegerv(GL_MAJOR_VERSION, &TempMajor);
	glGetIntegerv(GL_MINOR_VERSION, &TempMinor);
	if ((glGetError() == GL_NO_ERROR) && (TempMajor != 0))
		{
		Version_Set(&crGL4Information.OpenGLVersion, TempMajor, TempMinor);
		}
	else
		{
		// Still don't have the version. Try to get it with GL_VERSION...
		const GLubyte *GLVersionString = glGetString(GL_VERSION);
		if (GLVersionString == NULL)
			return false;
		// < v3.0; resort to the old-fashioned way.
		LOG_DEBUG("Reported GL version string : '%s'", GLVersionString);
		if (Version_ParseString(&crGL4Information.OpenGLVersion, (const char *)GLVersionString) == false)
			return false;
		}
	LOG_DEBUG("OpenGL version %d.%d", crGL4Information.OpenGLVersion.major, crGL4Information.OpenGLVersion.minor);

	/**
	OpenGL  #version    latest spec version number  Note
	2.0     100     1.10.59
	2.1     120     1.20.8
	3.0     130     1.30.10
	3.1     140     1.40.08
	3.2     150     1.50.11     added Geometry Shaders
	3.3     330     3.30.6
	4.0     400     4.00.9  added both Tessellation Shaders
	4.1     410     4.10.6
	4.2     420     4.20.11
	4.3     430     4.30.6   added Compute Shaders
	*/

	// Safe assumptions about GLSL
	if (version_GreaterThanOrEqual(crGL4Information.OpenGLVersion, 2, 0)) Version_Set(&crGL4Information.GLSLVersion, 1, 0);
	if (version_GreaterThanOrEqual(crGL4Information.OpenGLVersion, 2, 1)) Version_Set(&crGL4Information.GLSLVersion, 1, 2);
	if (version_GreaterThanOrEqual(crGL4Information.OpenGLVersion, 3, 0)) Version_Set(&crGL4Information.GLSLVersion, 1, 3);
	if (version_GreaterThanOrEqual(crGL4Information.OpenGLVersion, 3, 1)) Version_Set(&crGL4Information.GLSLVersion, 1, 4);
	if (version_GreaterThanOrEqual(crGL4Information.OpenGLVersion, 3, 2)) Version_Set(&crGL4Information.GLSLVersion, 1, 5);
	if (version_GreaterThanOrEqual(crGL4Information.OpenGLVersion, 3, 3)) crGL4Information.GLSLVersion = crGL4Information.OpenGLVersion;

	const GLubyte *GLSLVersionString = glGetString(GL_SHADING_LANGUAGE_VERSION);
	if (GLSLVersionString != NULL)
		{
		LOG_DEBUG("Reported GLSL version string : '%s'", GLSLVersionString);
		if (Version_ParseString(&crGL4Information.GLSLVersion, (const char *)GLSLVersionString) == false)
			LOG_ERROR("Error parsing GLSL version string. Assuming safe values");
		}
	LOG_DEBUG("GLSL version %d.%d", crGL4Information.GLSLVersion.major, crGL4Information.GLSLVersion.minor);

	const GLubyte *String = glGetString(GL_RENDERER);
	if ((String != NULL) && (strlen((char *)String) > 0))
		LOG_DEBUG("OpenGL renderer string : '%s'", String);

	for (GLenum iterator = 0; iterator < /*std::numeric_limits<unsigned>::max ()*/3; ++iterator)
		{
		String = glGetString(GL_VENDOR + iterator);
		if ((String != NULL) && (strlen((char *)String) > 0))
			LOG_DEBUG("OpenGL vendor string : '%s'", String);
		}

	crGL4Information.ExtensionCount = 0;
	crGL4Information.Extensions = NULL;
	// Method 1 - glGetStringi is only guaranteed to be available after glcore/gles 3.0
	if (version_GreaterThanOrEqual(crGL4Information.OpenGLVersion, 3, 0))
		{
		// Try to get extensions using the index
		int ExtensionCount = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &ExtensionCount);
		if ((glGetError() == GL_NO_ERROR) && (ExtensionCount != 0) && (glGetStringi != NULL))
			{
			crGL4Information.ExtensionCount = ExtensionCount;
			crGL4Information.Extensions = calloc(ExtensionCount, sizeof(char *));
			for (int ExtensionIndex = 0; ExtensionIndex < ExtensionCount; ++ExtensionIndex)
				{
				const char *CurrentExtension = (const char *)glGetStringi(GL_EXTENSIONS, ExtensionIndex);
				if (CurrentExtension == NULL) continue;
				crGL4Information.Extensions[ExtensionIndex] = strdup(CurrentExtension);
				}
			}
		}
	// Method 2
	// Profiles >= 3.0 are not guaranteed to have GL_EXTENSIONS support for glGetString
	if (crGL4Information.Extensions == NULL)
		{
		const char *ExtensionString, *Begin, *End;
		ExtensionString = (const char *)glGetString(GL_EXTENSIONS);
		if (ExtensionString == NULL)
			return false;
		if (glGetError() == GL_NO_ERROR)
			{
			unsigned Length = (unsigned)strlen(ExtensionString);
			unsigned ExtensionIndex = 0;
			for (Begin = ExtensionString; Begin < ExtensionString + Length; Begin = End + 1)
				{
				End = strchr(Begin, ' ');
				if (End == NULL) End = ExtensionString + Length;
				crGL4Information.Extensions[ExtensionIndex] = strndup(Begin, End - Begin + 1);
				++ExtensionIndex;
				}
			}
		}

	LOG_DEBUG("Detected extensions: %lu", crGL4Information.ExtensionCount);
	for (unsigned Index = 0; Index < crGL4Information.ExtensionCount; ++Index)
		LOG_DEBUG("%s", crGL4Information.Extensions[Index]);

	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &crGL4Information.MaxTextureUnits);

	crGL4Information.DirectStateAccessAvailable = (version_GreaterThanOrEqual(crGL4Information.OpenGLVersion, 4, 5) || (crGL4IsExtensionAvailable("GL_ARB_direct_state_access")));
	crGL4Information.DirectStateAccessEnabled = false;

	crGL4Information.SeamlessCubeMapAvailable = (version_GreaterThanOrEqual(crGL4Information.OpenGLVersion, 3, 2) || (crGL4IsExtensionAvailable("ARB_seamless_cube_map")));
	crGL4Information.SeamlessCubeMapEnabled = false;
	return true;
	}

bool crGL4InitializeRenderer(const crRendererConfiguration NewConfiguration)
	{
	if (crGL4InitializeGLContextFunctions() == false)
		return false;

	crWindowHandle NewWindowHandle = crCreateNewWindow(NewConfiguration.InitialWindowDescriptor);
	if (!NewWindowHandle)
		return false;

	crOpenGLContext OpenGLContext = crGL4CreateGLContext(NewWindowHandle, NewConfiguration);
	if (OpenGLContext == NULL)
		goto OnError;

	gladLoadGL();
	DetectOpenGLInformation();

	if (Version_LesserThan(crGL4Information.OpenGLVersion, 4, 0))
		{
		LOG_ERROR("OpenGL Core 4.0+ needed");
		goto OnError;
		}

	// If necessary, create a VAO
	glGenVertexArrays(1, &GeneralVAO);
	glBindVertexArray(GeneralVAO);

	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0);
	glEnable(GL_PROGRAM_POINT_SIZE);

	if (crGL4Information.SeamlessCubeMapAvailable)
		{
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		crGL4Information.SeamlessCubeMapEnabled = true;
		LOG_DEBUG("Seamless cube map textures enabled");
		}

	if (crGL4Information.DirectStateAccessAvailable)
		{
		crGL4Information.DirectStateAccessEnabled = true;
		LOG_DEBUG("Direct State Access enabled");
		}

	if (crGL4CheckError() == false)
		goto OnError;

	LOG_DEBUG("Max Texture Units - %u", crGL4Information.MaxTextureUnits);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(OpenGLMessageCallback, NULL);
	return true;
OnError:
	if (GeneralVAO != 0)
		{
		glDeleteVertexArrays(1, &GeneralVAO);
		GeneralVAO = 0;
		}
	if (NewWindowHandle)
		crDestroyWindow(NewWindowHandle);
	return false;
	}

bool crGL4ShutdownRenderer(void)
	{
	if (GeneralVAO != 0)
		{
		glDeleteVertexArrays(1, &GeneralVAO);
		GeneralVAO = 0;
		}
	return true;
	}

#if 0

bool DisplayWindow(const RenderWindowHandle &WindowHandle)
	{
	StateCache::ConfigureScissor(ScissorSettings());
	StateCache::ConfigureViewport(ViewportSettings());
	StateCache::ConfigureStencil(StencilBufferSettings());
	WindowManager::SwapGLWindowBuffer(WindowHandle);
	return crGL4CheckError();
	}

bool RunCommand(const RenderCommand &Command)
	{
	if (!Command.Shader)
		return false;
	if (!Command.VertexCount)
		return false;

	// Disable active vertex attributes
	if ((ActiveShader) && (ActiveShader != Command.Shader))
		{
		ShaderInfo *OldShaderInfo = &Shaders[ActiveShader.GetKey()];
		for (auto &AttributeIterator : OldShaderInfo->Attributes)
			{
			if (AttributeIterator.Enabled == true)
				{
				unsigned RowCount = 1;
				if (AttributeIterator.Type == ShaderAttributeType::Matrix4)
					RowCount = 4;
				AttributeIterator.Enabled = false;
				for (unsigned Row = 0; Row < RowCount; ++Row)
					glDisableVertexAttribArray(AttributeIterator.OpenGLID + Row);
				}
			}
		}

	// Activate new shader
	ShaderInfo *ShaderInformation = &Shaders[Command.Shader.GetKey()];
	if (ActiveShader != Command.Shader)
		{
		glUseProgram(ShaderInformation->OpenGLID);
		ActiveShader = Command.Shader;
		if (crGL4CheckError() == false)
			return false;
		}

	StateCache::ApplyState(Command.State);
	if (crGL4CheckError() == false)
		return false;

	// Set all uniforms
	unsigned UniformBlockBindingIndex = 0;
	for (auto &Iterator : Command.UniformValues)
		{
		if (!Iterator.UniformHandle)
			return false;
		UniformInfo *UniformInformation = &ShaderInformation->Uniforms[Iterator.UniformHandle.GetKey()];

		if (ShaderInformation->Uniforms[UniformIndex].CurrentValue.Equals(Iterator.UniformValue, ShaderInformation->Uniforms[UniformIndex].Type) == true)
			continue;

		switch (ShaderInformation->Uniforms[UniformIndex].Type)
			{
#define CASE_TYPE(TYPENAME,FUNCTION,DSAFUNCTION)\
        case ShaderUniformType::TYPENAME:\
            {\
			if ( OpenGLInformation.DirectStateAccessEnabled)\
				DSAFUNCTION( ShaderInformation->OpenGLID, ShaderInformation->Uniforms[UniformIndex].OpenGLID, Iterator.UniformValue.TYPENAME##Value );\
			else\
				FUNCTION ( ShaderInformation->Uniforms[UniformIndex].OpenGLID, Iterator.UniformValue.TYPENAME##Value );\
            break;\
            }
#define CASE_TYPE_VEC(TYPENAME,FUNCTION,DSAFUNCTION)\
        case ShaderUniformType::TYPENAME:\
            {\
			if(OpenGLInformation.DirectStateAccessEnabled)\
				DSAFUNCTION( ShaderInformation->OpenGLID, ShaderInformation->Uniforms[UniformIndex].OpenGLID, 1, glm::value_ptr(Iterator.UniformValue.TYPENAME##Value) );\
			else\
				FUNCTION ( ShaderInformation->Uniforms[UniformIndex].OpenGLID, 1, glm::value_ptr(Iterator.UniformValue.TYPENAME##Value) );\
            break;\
            }

				CASE_TYPE(Float, glUniform1f, glProgramUniform1f);
				CASE_TYPE_VEC(Float2, glUniform2fv, glProgramUniform2fv);
				CASE_TYPE_VEC(Float3, glUniform3fv, glProgramUniform3fv);
				CASE_TYPE_VEC(Float4, glUniform4fv, glProgramUniform4fv);

				CASE_TYPE(Integer, glUniform1i, glProgramUniform1i);
				CASE_TYPE_VEC(Integer2, glUniform2iv, glProgramUniform2iv);
				CASE_TYPE_VEC(Integer3, glUniform3iv, glProgramUniform3iv);
				CASE_TYPE_VEC(Integer4, glUniform4iv, glProgramUniform4iv);

				CASE_TYPE(UnsignedInteger, glUniform1ui, glProgramUniform1ui);
				CASE_TYPE_VEC(UnsignedInteger2, glUniform2uiv, glProgramUniform2uiv);
				CASE_TYPE_VEC(UnsignedInteger3, glUniform3uiv, glProgramUniform3uiv);
				CASE_TYPE_VEC(UnsignedInteger4, glUniform4uiv, glProgramUniform4uiv);
#undef CASE_TYPE
#undef CASE_TYPE_VEC

			case ShaderUniformType::Matrix3:
				if (OpenGLInformation.DirectStateAccessEnabled)
					glProgramUniformMatrix3fv(ShaderInformation->OpenGLID, ShaderInformation->Uniforms[UniformIndex].OpenGLID, 1, GL_FALSE, glm::value_ptr(Iterator.UniformValue.Matrix3Value));
				else
					glUniformMatrix3fv(ShaderInformation->Uniforms[UniformIndex].OpenGLID, 1, GL_FALSE, glm::value_ptr(Iterator.UniformValue.Matrix3Value));
				break;
			case ShaderUniformType::Matrix4:
				if (OpenGLInformation.DirectStateAccessEnabled)
					glProgramUniformMatrix4fv(ShaderInformation->OpenGLID, ShaderInformation->Uniforms[UniformIndex].OpenGLID, 1, GL_FALSE, glm::value_ptr(Iterator.UniformValue.Matrix4Value));
				else
					glUniformMatrix4fv(ShaderInformation->Uniforms[UniformIndex].OpenGLID, 1, GL_FALSE, glm::value_ptr(Iterator.UniformValue.Matrix4Value));
				break;
			case ShaderUniformType::Bool:
				if (OpenGLInformation.DirectStateAccessEnabled)
					glProgramUniform1i(ShaderInformation->OpenGLID, ShaderInformation->Uniforms[UniformIndex].OpenGLID, Iterator.UniformValue.BoolValue);
				else
					glUniform1i(ShaderInformation->Uniforms[UniformIndex].OpenGLID, Iterator.UniformValue.BoolValue);
				break;
			case ShaderUniformType::Block:
				{
				ShaderBufferInfo SBInfo = ShaderBuffers[Iterator.UniformValue.BlockValue.GetKey()];
				glUniformBlockBinding(ShaderInformation->OpenGLID, ShaderInformation->Uniforms[UniformIndex].OpenGLID, UniformBlockBindingIndex);
				glBindBufferRange(GL_UNIFORM_BUFFER, UniformBlockBindingIndex, SBInfo.OpenGLID, 0, SBInfo.DataSize);
				++UniformBlockBindingIndex;
				break;
				}
			default:
				throw std::runtime_error("Unhandled shader uniform type");
			}
		ShaderInformation->Uniforms[UniformIndex].CurrentValue = Iterator.UniformValue;
		}
	if (crGL4CheckError() == false)
		return false;

	// Bind all attribute buffers
	for (auto &Iterator : Command.ShaderBufferBindings)
		{
		AttributeInfo *AttributeInformation = &(ShaderInformation->Attributes[Iterator.AttributeHandle.GetKey()]);
		ShaderBufferInfo *BufferInformation = &(ShaderBuffers[Iterator.DataStream.BufferHandle.GetKey()]);

		if (BufferInformation->MappedPointer)
			{
			OpenGL::UnmapShaderBuffer(Iterator.DataStream.BufferHandle);
			}

		glBindBuffer(GL_ARRAY_BUFFER, BufferInformation->OpenGLID);
		switch (AttributeInformation->Type)
			{
			case ShaderAttributeType::Matrix4:
				{
				if (AttributeInformation->Enabled == false)
					{
					for (unsigned RowCount = 0; RowCount < 4; ++RowCount)
						{
						glEnableVertexAttribArray(AttributeInformation->OpenGLID + RowCount);
						glVertexAttribDivisor(AttributeInformation->OpenGLID + RowCount, Iterator.DataStream.PerInstance ? 1 : 0);
						crGL4CheckError();
						}
					AttributeInformation->Enabled = true;
					}
				for (unsigned RowCount = 0; RowCount < 4; ++RowCount)
					{
					glVertexAttribPointer(AttributeInformation->OpenGLID + RowCount,
					                      4,
					                      Translate(Iterator.DataStream.ComponentType),
					                      Iterator.DataStream.NormalizeData,
					                      (GLsizei)Iterator.DataStream.Stride,
					                      (void *)((uint8_t *)Iterator.DataStream.StartOffset + (sizeof(glm::vec4) * RowCount)));
					crGL4CheckError();
					}
				break;
				}
			case ShaderAttributeType::Float:
			case ShaderAttributeType::Float2:
			case ShaderAttributeType::Float3:
			case ShaderAttributeType::Float4:

			case ShaderAttributeType::Bool:
			case ShaderAttributeType::Bool2:
			case ShaderAttributeType::Bool3:
			case ShaderAttributeType::Bool4:
			case ShaderAttributeType::UnsignedInteger:
			case ShaderAttributeType::UnsignedInteger2:
			case ShaderAttributeType::UnsignedInteger3:
			case ShaderAttributeType::UnsignedInteger4:
			case ShaderAttributeType::Integer:
			case ShaderAttributeType::Integer2:
			case ShaderAttributeType::Integer3:
			case ShaderAttributeType::Integer4:
				{
				if (AttributeInformation->Enabled == false)
					{
					glEnableVertexAttribArray(AttributeInformation->OpenGLID);
					glVertexAttribDivisor(AttributeInformation->OpenGLID, Iterator.DataStream.PerInstance ? 1 : 0);
					AttributeInformation->Enabled = true;
					}
				glBindBuffer(GL_ARRAY_BUFFER, BufferInformation->OpenGLID);

				// Is the attribute in the shader an int, meaning I should convert these values?
				bool AttributeIsInt = (((AttributeInformation->Type >= ShaderAttributeType::Bool) && (AttributeInformation->Type <= ShaderAttributeType::Bool4)) ||
				                       ((AttributeInformation->Type >= ShaderAttributeType::UnsignedInteger) && (AttributeInformation->Type <= ShaderAttributeType::Integer4)));
				if ((Iterator.DataStream.ComponentType != ShaderBufferComponentType::Float) && (AttributeIsInt))
					{
					glVertexAttribIPointer(AttributeInformation->OpenGLID,
					                       (GLint)Iterator.DataStream.ComponentsPerElement,
					                       Translate(Iterator.DataStream.ComponentType),
					                       (GLsizei)Iterator.DataStream.Stride,
					                       (void *)Iterator.DataStream.StartOffset);
					}
				else
					{
					glVertexAttribPointer(AttributeInformation->OpenGLID,
					                      (GLint)Iterator.DataStream.ComponentsPerElement,
					                      Translate(Iterator.DataStream.ComponentType),
					                      Iterator.DataStream.NormalizeData,
					                      (GLsizei)Iterator.DataStream.Stride,
					                      (void *)Iterator.DataStream.StartOffset);
					}
				break;
				}
			default:
				{
				throw std::runtime_error("Unimplemented shader attribute type");
				break;
				}
			}
		}

	if (crGL4CheckError() == false)
		return false;

	// Bind all textures
	unsigned TextureBindingIndex = 0;
	for (auto &Iterator : Command.TextureBindings)
		{
		UniformInfo *UniformInformation = &ShaderInformation->Uniforms[Iterator.UniformHandle.GetKey()];

		GLint DesiredSWrap = Translate(Iterator.BindSettings.WrapSettings.Horizontal);
		GLint DesiredTWrap = Translate(Iterator.BindSettings.WrapSettings.Vertical);
		GLint DesiredMinFilter = Translate(Iterator.BindSettings.FilterSettings.MinFilter);
		GLint DesiredMagFilter = Translate(Iterator.BindSettings.FilterSettings.MagFilter);
		TextureInfo *TextureInformation = &Textures[Iterator.BindSettings.Handle.GetKey()];

		GLint TextureType;
		switch (TextureInformation->Type)
			{
			case TextureType::Texture2D:
				{
				TextureType = GL_TEXTURE_2D;
				break;
				}
			case TextureType::TextureCubeMap:
				{
				TextureType = GL_TEXTURE_CUBE_MAP;
				break;
				}
			default:
				return false;
			}

		if (OpenGLInformation.DirectStateAccessEnabled)
			{
			glBindTextureUnit(TextureBindingIndex, TextureInformation->OpenGLID);
			if (TextureInformation->GLSWrap != DesiredSWrap)
				glTextureParameteri(TextureInformation->OpenGLID, GL_TEXTURE_WRAP_S, DesiredSWrap);
			if (TextureInformation->GLTWrap != DesiredTWrap)
				glTextureParameteri(TextureInformation->OpenGLID, GL_TEXTURE_WRAP_T, DesiredTWrap);
			if (TextureInformation->GLMinFilter != DesiredMinFilter)
				glTextureParameteri(TextureInformation->OpenGLID, GL_TEXTURE_MIN_FILTER, DesiredMinFilter);
			if (TextureInformation->GLMagFilter != DesiredMagFilter)
				glTextureParameteri(TextureInformation->OpenGLID, GL_TEXTURE_MAG_FILTER, DesiredMagFilter);
			glProgramUniform1i(ShaderInformation->OpenGLID, ShaderInformation->Uniforms[UniformIndex].OpenGLID, (GLint)TextureBindingIndex);
			}
		else
			{
			glActiveTexture(GL_TEXTURE0 + TextureBindingIndex); // Multitexture index
			glBindTexture(TextureType, TextureInformation->OpenGLID);
			if (TextureInformation->GLSWrap != DesiredSWrap)
				glTexParameteri(TextureType, GL_TEXTURE_WRAP_S, DesiredSWrap);
			if (TextureInformation->GLTWrap != DesiredTWrap)
				glTexParameteri(TextureType, GL_TEXTURE_WRAP_T, DesiredTWrap);
			if (TextureInformation->GLMinFilter != DesiredMinFilter)
				glTexParameteri(TextureType, GL_TEXTURE_MIN_FILTER, DesiredMinFilter);
			if (TextureInformation->GLMagFilter != DesiredMagFilter)
				glTexParameteri(TextureType, GL_TEXTURE_MAG_FILTER, DesiredMagFilter);
			glUniform1i(ShaderInformation->Uniforms[UniformIndex].OpenGLID, (GLint)TextureBindingIndex);
			}
		if (crGL4CheckError() == false)
			{
			LOG_ERROR("Error activating texture");
			return false;
			}
		TextureInformation->GLSWrap = DesiredSWrap;
		TextureInformation->GLTWrap = DesiredTWrap;
		TextureInformation->GLMinFilter = DesiredMinFilter;
		TextureInformation->GLMagFilter = DesiredMagFilter;

		++TextureBindingIndex;
		}
	if (crGL4CheckError() == false)
		return false;

	// Finally, issue the draw call
	if (Command.IndexBufferStream.BufferHandle)
		{
		static size_t Sizes[] = { sizeof(float), sizeof(uint8_t), sizeof(uint16_t), sizeof(uint32_t) };

		ShaderBufferInfo *BufferToUse = &ShaderBuffers[Command.IndexBufferStream.BufferHandle.GetKey()];
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferToUse->OpenGLID);
		glDrawElementsInstanced(Translate(Command.Primitive),
		                        static_cast <GLsizei> (Command.VertexCount),
		                        Translate(Command.IndexBufferStream.ComponentType),
		                        reinterpret_cast <void *> (Command.IndexBufferStream.StartOffset + Command.StartVertex * Sizes[(int)Command.IndexBufferStream.ComponentType]),
		                        static_cast <GLsizei> (Command.InstanceCount));
		}
	else
		{
		glDrawArraysInstanced(Translate(Command.Primitive),
		                      static_cast<GLint> (Command.StartVertex),
		                      static_cast <GLsizei> (Command.VertexCount),
		                      static_cast <GLsizei> (Command.InstanceCount));
		}
	return crGL4CheckError();
	}
#endif

bool crGL4IsExtensionAvailable(const char *Extension)
	{
	for (unsigned Index = 0; Index < crGL4Information.ExtensionCount; ++Index)
		{
		if (strcmp(Extension, crGL4Information.Extensions[Index]) == 0)
			return true;
		}
	return false;
	}

bool crGL4EnableDirectStateAccess(const bool NewState)
	{
	crGL4Information.DirectStateAccessEnabled = NewState;
	return true;
	}

bool crGL4IsDirectStateAccessEnabled(void)
	{
	return crGL4Information.DirectStateAccessEnabled;
	}

bool crGL4StartRenderToWindow(const crWindowHandle WindowHandle)
	{
	crActivateWindow(WindowHandle);
	crGL4MakeGLContextActive(WindowHandle);

	uvec2 WindowDimensions;
	crGetWindowClientAreaDimensions(WindowHandle, &WindowDimensions);
	crGL4SetDefaultViewportSize(WindowDimensions);

	static crScissorSettings DefaultScissorSettings = { 0 };
	static crViewportSettings DefaultViewportSettings = { 0 };
	static crStencilBufferSettings DefaultStencilSettings = { 0 };
	crGL4ConfigureScissor(&DefaultScissorSettings);
	crGL4ConfigureViewport(&DefaultViewportSettings);
	crGL4ConfigureStencil(&DefaultStencilSettings);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return crGL4CheckError();
	}

bool crGL4DisplayFramebuffer(const crFramebufferHandle FramebufferHandle, const crWindowHandle WindowHandle)
	{
	crActivateWindow(WindowHandle);
	crGL4MakeGLContextActive(WindowHandle);

	uvec2 WindowDimensions;
	// Reset scissor, viewport and stencil to the full window
	crGetWindowClientAreaDimensions(WindowHandle, &WindowDimensions);
	crGL4SetDefaultViewportSize(WindowDimensions);
	static crScissorSettings DefaultScissorSettings = { 0 };
	static crViewportSettings DefaultViewportSettings = { 0 };
	static crStencilBufferSettings DefaultStencilSettings = { 0 };
	crGL4ConfigureScissor(&DefaultScissorSettings);
	crGL4ConfigureViewport(&DefaultViewportSettings);
	crGL4ConfigureStencil(&DefaultStencilSettings);
	crGL4MakeGLContextActive(WindowHandle);

	crGL4InternalFramebufferInfo *FramebufferInformation = (crGL4InternalFramebufferInfo *)PointerList_GetNodeData(FramebufferHandle);
	if (FramebufferInformation->OpenGLID != 0)
		{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, FramebufferInformation->OpenGLID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(
		    0, 0, FramebufferInformation->Dimensions.x, FramebufferInformation->Dimensions.y,
		    0, 0, FramebufferInformation->Dimensions.x, FramebufferInformation->Dimensions.y,
		    GL_COLOR_BUFFER_BIT, GL_NEAREST);
		// Restore old framebuffer binding
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FramebufferInformation->OpenGLID);
		}

	return true;
	}

bool crGL4DisplayWindow(const crWindowHandle WindowHandle)
	{
	crActivateWindow(WindowHandle);
	uvec2 WindowDimensions;
	crGetWindowClientAreaDimensions(WindowHandle, &WindowDimensions);
	crGL4SetDefaultViewportSize(WindowDimensions);

	static crScissorSettings DefaultScissorSettings = { 0 };
	static crViewportSettings DefaultViewportSettings = { 0 };
	static crStencilBufferSettings DefaultStencilSettings = { 0 };
	crGL4ConfigureScissor(&DefaultScissorSettings);
	crGL4ConfigureViewport(&DefaultViewportSettings);
	crGL4ConfigureStencil(&DefaultStencilSettings);
	crGL4SwapGLWindowBuffer(WindowHandle);
	return crGL4CheckError();
	}

static bool ShaderUniformValueIsEqual(const crShaderUniformType Type, const crShaderUniformValue *Value1, const crShaderUniformValue *Value2)
	{
	switch (Type)
		{
		case crShaderUniformType_Bool:
			return Value1->BoolValue == Value2->BoolValue;
		case crShaderUniformType_Bool2:
			return math_ivec2_equals(Value1->Bool2Value, Value2->Bool2Value);
		case crShaderUniformType_Bool3:
			return math_ivec3_equals(Value1->Bool3Value, Value2->Bool3Value);
		case crShaderUniformType_Bool4:
			return math_ivec4_equals(Value1->Bool4Value, Value2->Bool4Value);
		case crShaderUniformType_Float:
			return Value1->FloatValue == Value2->FloatValue;
		case crShaderUniformType_Float2:
			return math_vec2_equals(Value1->Float2Value, Value2->Float2Value);
		case crShaderUniformType_Float3:
			return math_vec3_equals(Value1->Float3Value, Value2->Float3Value);
		case crShaderUniformType_Float4:
			return math_vec4_equals(Value1->Float4Value, Value2->Float4Value);
		case crShaderUniformType_UnsignedInteger:
			return Value1->UnsignedIntegerValue == Value2->UnsignedIntegerValue;
		case crShaderUniformType_UnsignedInteger2:
			return math_uvec2_equals(Value1->UnsignedInteger2Value, Value2->UnsignedInteger2Value);
		case crShaderUniformType_UnsignedInteger3:
			return math_uvec3_equals(Value1->UnsignedInteger3Value, Value2->UnsignedInteger3Value);
		case crShaderUniformType_UnsignedInteger4:
			return math_uvec4_equals(Value1->UnsignedInteger4Value, Value2->UnsignedInteger4Value);
		case crShaderUniformType_Integer:
			return Value1->IntegerValue == Value2->IntegerValue;
		case crShaderUniformType_Integer2:
			return math_ivec2_equals(Value1->Integer2Value, Value2->Integer2Value);
		case crShaderUniformType_Integer3:
			return math_ivec3_equals(Value1->Integer3Value, Value2->Integer3Value);
		case crShaderUniformType_Integer4:
			return math_ivec4_equals(Value1->Integer4Value, Value2->Integer4Value);
		case crShaderUniformType_Matrix2:
			return math_mat2_equals(Value1->Matrix2Value, Value2->Matrix2Value);
		case crShaderUniformType_Matrix3:
			return math_mat3_equals(Value1->Matrix3Value, Value2->Matrix3Value);
		case crShaderUniformType_Matrix4:
			return math_mat4_equals(Value1->Matrix4Value, Value2->Matrix4Value);
		case crShaderUniformType_Block:
			return Value1->BlockValue == Value2->BlockValue;
		}

	return false;
	}

static bool CopyShaderUniformValue(const crShaderUniformType Type, const crShaderUniformValue *Input, crShaderUniformValue *Output)
	{
	switch (Type)
		{
		case crShaderUniformType_Bool:
			Output->BoolValue = Input->BoolValue;
			break;
		case crShaderUniformType_Bool2:
			math_ivec2_copy(&Output->Bool2Value, Input->Bool2Value);
			break;
		case crShaderUniformType_Bool3:
			math_ivec3_copy(&Output->Bool3Value, Input->Bool3Value);
			break;
		case crShaderUniformType_Bool4:
			math_ivec4_copy(&Output->Bool4Value, Input->Bool4Value);
			break;
		case crShaderUniformType_Float:
			Output->FloatValue = Input->FloatValue;
			break;
		case crShaderUniformType_Float2:
			math_vec2_copy(&Output->Float2Value, Input->Float2Value);
			break;
		case crShaderUniformType_Float3:
			math_vec3_copy(&Output->Float3Value, Input->Float3Value);
			break;
		case crShaderUniformType_Float4:
			math_vec4_copy(&Output->Float4Value, Input->Float4Value);
			break;
		case crShaderUniformType_UnsignedInteger:
			Output->UnsignedIntegerValue = Input->UnsignedIntegerValue;
			break;
		case crShaderUniformType_UnsignedInteger2:
			math_uvec2_copy(&Output->UnsignedInteger2Value, Input->UnsignedInteger2Value);
			break;
		case crShaderUniformType_UnsignedInteger3:
			math_uvec3_copy(&Output->UnsignedInteger3Value, Input->UnsignedInteger3Value);
			break;
		case crShaderUniformType_UnsignedInteger4:
			math_uvec4_copy(&Output->UnsignedInteger4Value, Input->UnsignedInteger4Value);
			break;
		case crShaderUniformType_Integer:
			Output->IntegerValue = Input->IntegerValue;
			break;
		case crShaderUniformType_Integer2:
			math_ivec2_copy(&Output->Integer2Value, Input->Integer2Value);
			break;
		case crShaderUniformType_Integer3:
			math_ivec3_copy(&Output->Integer3Value, Input->Integer3Value);
			break;
		case crShaderUniformType_Integer4:
			math_ivec4_copy(&Output->Integer4Value, Input->Integer4Value);
			break;
		case crShaderUniformType_Matrix2:
			math_mat2_copy(&Output->Matrix2Value, Input->Matrix2Value);
			break;
		case crShaderUniformType_Matrix3:
			math_mat3_copy(&Output->Matrix3Value, Input->Matrix3Value);
			break;
		case crShaderUniformType_Matrix4:
			math_mat4_copy(&Output->Matrix4Value, Input->Matrix4Value);
			break;
		case crShaderUniformType_Block:
			Output->BlockValue = Input->BlockValue;
			break;
		default:
			return false;
		}

	return true;
	}

static bool SetOpenGLShaderUniformValue(const crGL4InternalShaderInfo *ShaderInformation, const crShaderUniformHandle UniformHandle, crShaderUniformValue *Value, unsigned *UniformBlockBindingIndex)
	{
	switch (ShaderInformation->Uniforms[UniformHandle].Type)
		{
#define CASE_TYPE(TYPENAME,FUNCTION,DSAFUNCTION)\
        case crShaderUniformType_##TYPENAME:\
            {\
			if ( crGL4Information.DirectStateAccessEnabled)\
				DSAFUNCTION( ShaderInformation->OpenGLID, ShaderInformation->Uniforms[UniformHandle].OpenGLID, Value->TYPENAME##Value );\
			else\
				FUNCTION ( ShaderInformation->Uniforms[UniformHandle].OpenGLID, Value->TYPENAME##Value );\
            break;\
            }
#define CASE_TYPE_VEC(TYPENAME,FUNCTION,DSAFUNCTION)\
        case crShaderUniformType_##TYPENAME:\
            {\
			if(crGL4Information.DirectStateAccessEnabled)\
				DSAFUNCTION( ShaderInformation->OpenGLID, ShaderInformation->Uniforms[UniformHandle].OpenGLID, 1, Value->TYPENAME##Value.raw );\
			else\
				FUNCTION ( ShaderInformation->Uniforms[UniformHandle].OpenGLID, 1, Value->TYPENAME##Value.raw );\
            break;\
            }

			CASE_TYPE(Bool, glUniform1i, glProgramUniform1i);
			CASE_TYPE_VEC(Bool2, glUniform2iv, glProgramUniform2iv);
			CASE_TYPE_VEC(Bool3, glUniform3iv, glProgramUniform3iv);
			CASE_TYPE_VEC(Bool4, glUniform4iv, glProgramUniform4iv);

			CASE_TYPE(Float, glUniform1f, glProgramUniform1f);
			CASE_TYPE_VEC(Float2, glUniform2fv, glProgramUniform2fv);
			CASE_TYPE_VEC(Float3, glUniform3fv, glProgramUniform3fv);
			CASE_TYPE_VEC(Float4, glUniform4fv, glProgramUniform4fv);

			CASE_TYPE(Integer, glUniform1i, glProgramUniform1i);
			CASE_TYPE_VEC(Integer2, glUniform2iv, glProgramUniform2iv);
			CASE_TYPE_VEC(Integer3, glUniform3iv, glProgramUniform3iv);
			CASE_TYPE_VEC(Integer4, glUniform4iv, glProgramUniform4iv);

			CASE_TYPE(UnsignedInteger, glUniform1ui, glProgramUniform1ui);
			CASE_TYPE_VEC(UnsignedInteger2, glUniform2uiv, glProgramUniform2uiv);
			CASE_TYPE_VEC(UnsignedInteger3, glUniform3uiv, glProgramUniform3uiv);
			CASE_TYPE_VEC(UnsignedInteger4, glUniform4uiv, glProgramUniform4uiv);
#undef CASE_TYPE
#undef CASE_TYPE_VEC

		case crShaderUniformType_Matrix3:
			{
			if (crGL4Information.DirectStateAccessEnabled)
				glProgramUniformMatrix3fv(ShaderInformation->OpenGLID, ShaderInformation->Uniforms[UniformHandle].OpenGLID, 1, GL_FALSE, Value->Matrix3Value.raw);
			else
				glUniformMatrix3fv(ShaderInformation->Uniforms[UniformHandle].OpenGLID, 1, GL_FALSE, Value->Matrix3Value.raw);
			break;
			}
		case crShaderUniformType_Matrix4:
			{
			if (crGL4Information.DirectStateAccessEnabled)
				glProgramUniformMatrix4fv(ShaderInformation->OpenGLID, ShaderInformation->Uniforms[UniformHandle].OpenGLID, 1, GL_FALSE, Value->Matrix4Value.raw);
			else
				glUniformMatrix4fv(ShaderInformation->Uniforms[UniformHandle].OpenGLID, 1, GL_FALSE, Value->Matrix4Value.raw);
			break;
			}
		case crShaderUniformType_Block:
			{
			crGL4InternalShaderBufferInfo *ShaderBufferInformation = (crGL4InternalShaderBufferInfo *)PointerList_GetNodeData(Value->BlockValue);
			glUniformBlockBinding(ShaderInformation->OpenGLID, ShaderInformation->Uniforms[UniformHandle].OpenGLID, *UniformBlockBindingIndex);
			glBindBufferRange(GL_UNIFORM_BUFFER, *UniformBlockBindingIndex, ShaderBufferInformation->OpenGLID, 0, ShaderBufferInformation->DataSize);
			++(*UniformBlockBindingIndex);
			break;
			}
		default:
			LOG_ERROR("Unhandled shader uniform type");
			return false;
		}
	return true;
	}

bool crGL4RunCommand(const crRenderCommand Command)
	{
	if (!Command.Shader)
		return false;
	if (!Command.VertexCount)
		return false;

	// Deactivate previous shader's attributes
	if ((ActiveShader) && (ActiveShader != Command.Shader))
		{
		crGL4InternalShaderInfo *OldShaderInformation = (crGL4InternalShaderInfo *)PointerList_GetNodeData(ActiveShader);
		for (unsigned AttributeIndex = 0; AttributeIndex < OldShaderInformation->AttributeCount; ++AttributeIndex)
			{
			if (OldShaderInformation->Attributes[AttributeIndex].Enabled)
				{
				unsigned RowCount = 1;
				if (OldShaderInformation->Attributes[AttributeIndex].Type == crShaderAttributeType_Matrix4)
					RowCount = 4;
				OldShaderInformation->Attributes[AttributeIndex].Enabled = false;
				for (unsigned Row = 0; Row < RowCount; ++Row)
					glDisableVertexAttribArray(OldShaderInformation->Attributes[AttributeIndex].OpenGLID + Row);
				}
			}
		ActiveShader = NULL;
		}

	// Activate new shader
	crGL4InternalShaderInfo *ShaderInformation = (crGL4InternalShaderInfo *)PointerList_GetNodeData(Command.Shader);
	/*if (Command.ShaderBufferBindingCount != ShaderInformation->AttributeCount)
		return false;
	if (Command.UniformValueCount != ShaderInformation->UniformCount)
		return false;*/

	if (ActiveShader != Command.Shader)
		{
		glUseProgram(ShaderInformation->OpenGLID);
		ActiveShader = Command.Shader;
		if (crGL4CheckError() == false)
			return false;
		}

	if (crGL4ApplyState(&Command.State) == false)
		return false;

	// Set all uniforms
	unsigned UniformBlockBindingIndex = 0;
	for (unsigned UniformIndex = 0; UniformIndex < Command.UniformValueCount; ++UniformIndex)
		{
		if (ShaderUniformValueIsEqual(ShaderInformation->Uniforms[UniformIndex].Type, &ShaderInformation->Uniforms[UniformIndex].CurrentValue, &Command.UniformValues[UniformIndex].UniformValue) == true)
			continue;
		if (SetOpenGLShaderUniformValue(ShaderInformation, Command.UniformValues[UniformIndex].UniformHandle, &Command.UniformValues[UniformIndex].UniformValue, &UniformBlockBindingIndex) == false)
			return false;
		CopyShaderUniformValue(ShaderInformation->Uniforms[UniformIndex].Type, &Command.UniformValues[UniformIndex].UniformValue, &ShaderInformation->Uniforms[UniformIndex].CurrentValue);
		}

	if (crGL4CheckError() == false)
		return false;

	// bind all attributes
	for (unsigned BindingIndex = 0; BindingIndex < Command.ShaderBufferBindingCount; ++BindingIndex)
		{
		crGL4InternalShaderBufferInfo *ShaderBufferInformation = (crGL4InternalShaderBufferInfo *)PointerList_GetNodeData(Command.ShaderBufferBindings[BindingIndex].DataStream.BufferHandle);
		crGL4InternalAttributeInfo *AttributeInformation = &ShaderInformation->Attributes[Command.ShaderBufferBindings[BindingIndex].AttributeHandle]; //&ShaderInformation->Attributes[BindingIndex];
		if (ShaderBufferInformation->MappedPointer)
			{
			crGL4UnmapShaderBuffer(Command.ShaderBufferBindings[BindingIndex].DataStream.BufferHandle);
			}

		glBindBuffer(GL_ARRAY_BUFFER, ShaderBufferInformation->OpenGLID);
		switch (AttributeInformation->Type)
			{
			case crShaderAttributeType_Matrix4:
				{
				if (AttributeInformation->Enabled == false)
					{
					for (unsigned RowCount = 0; RowCount < 4; ++RowCount)
						{
						glEnableVertexAttribArray(AttributeInformation->OpenGLID + RowCount);
						glVertexAttribDivisor(AttributeInformation->OpenGLID + RowCount, Command.ShaderBufferBindings[BindingIndex].DataStream.PerInstance ? 1 : 0);
						crGL4CheckError();
						}
					AttributeInformation->Enabled = true;
					}
				for (unsigned RowCount = 0; RowCount < 4; ++RowCount)
					{
					glVertexAttribPointer(AttributeInformation->OpenGLID + RowCount,
					                      4,
					                      crGL4TranslateShaderBufferComponentType(Command.ShaderBufferBindings[BindingIndex].DataStream.ComponentType),
					                      Command.ShaderBufferBindings[BindingIndex].DataStream.NormalizeData,
					                      (GLsizei)Command.ShaderBufferBindings[BindingIndex].DataStream.Stride,
					                      (void *)((uint8_t *)Command.ShaderBufferBindings[BindingIndex].DataStream.StartOffset + (sizeof(vec4) * RowCount)));
					crGL4CheckError();
					}
				break;
				}
			case crShaderAttributeType_Float:
			case crShaderAttributeType_Float2:
			case crShaderAttributeType_Float3:
			case crShaderAttributeType_Float4:

			case crShaderAttributeType_Bool:
			case crShaderAttributeType_Bool2:
			case crShaderAttributeType_Bool3:
			case crShaderAttributeType_Bool4:
			case crShaderAttributeType_UnsignedInteger:
			case crShaderAttributeType_UnsignedInteger2:
			case crShaderAttributeType_UnsignedInteger3:
			case crShaderAttributeType_UnsignedInteger4:
			case crShaderAttributeType_Integer:
			case crShaderAttributeType_Integer2:
			case crShaderAttributeType_Integer3:
			case crShaderAttributeType_Integer4:
				{
				if (AttributeInformation->Enabled == false)
					{
					glEnableVertexAttribArray(AttributeInformation->OpenGLID);
					glVertexAttribDivisor(AttributeInformation->OpenGLID, Command.ShaderBufferBindings[BindingIndex].DataStream.PerInstance ? 1 : 0);
					AttributeInformation->Enabled = true;
					}
				glBindBuffer(GL_ARRAY_BUFFER, ShaderBufferInformation->OpenGLID);

				// Is the attribute in the shader an int, meaning I should convert these values?
				bool AttributeIsInt = (((AttributeInformation->Type >= crShaderAttributeType_Bool) && (AttributeInformation->Type <= crShaderAttributeType_Bool4)) ||
				                       ((AttributeInformation->Type >= crShaderAttributeType_UnsignedInteger) && (AttributeInformation->Type <= crShaderAttributeType_Integer4)));
				if ((Command.ShaderBufferBindings[BindingIndex].DataStream.ComponentType != crShaderBufferComponentType_Float) && (AttributeIsInt))
					{
					glVertexAttribIPointer(AttributeInformation->OpenGLID,
					                       (GLint)Command.ShaderBufferBindings[BindingIndex].DataStream.ComponentsPerElement,
					                       crGL4TranslateShaderBufferComponentType(Command.ShaderBufferBindings[BindingIndex].DataStream.ComponentType),
					                       (GLsizei)Command.ShaderBufferBindings[BindingIndex].DataStream.Stride,
					                       (void *)Command.ShaderBufferBindings[BindingIndex].DataStream.StartOffset);
					}
				else
					{
					glVertexAttribPointer(AttributeInformation->OpenGLID,
					                      (GLint)Command.ShaderBufferBindings[BindingIndex].DataStream.ComponentsPerElement,
					                      crGL4TranslateShaderBufferComponentType(Command.ShaderBufferBindings[BindingIndex].DataStream.ComponentType),
					                      Command.ShaderBufferBindings[BindingIndex].DataStream.NormalizeData,
					                      (GLsizei)Command.ShaderBufferBindings[BindingIndex].DataStream.Stride,
					                      (void *)Command.ShaderBufferBindings[BindingIndex].DataStream.StartOffset);
					}
				break;
				}
			default:
				{
				LOG_ERROR("Unimplemented shader attribute type");
				return false;
				}
			}
		}

	if (crGL4CheckError() == false)
		return false;

	unsigned TextureBindingIndex = 0;
	for (unsigned BindingIndex = 0; BindingIndex < Command.ShaderTextureBindingCount; ++BindingIndex)
		{
		crGL4InternalUniformInfo *UniformInformation = &ShaderInformation->Uniforms[Command.ShaderTextureBindings[BindingIndex].UniformHandle];
		crGL4InternalTextureInfo *TextureInformation = (crGL4InternalTextureInfo *)PointerList_GetNodeData(Command.ShaderTextureBindings[BindingIndex].BindSettings.Handle);

		GLint DesiredSWrap = crGL4TranslateTextureWrapMode(Command.ShaderTextureBindings[BindingIndex].BindSettings.WrapSettings.Horizontal);
		GLint DesiredTWrap = crGL4TranslateTextureWrapMode(Command.ShaderTextureBindings[BindingIndex].BindSettings.WrapSettings.Vertical);
		GLint DesiredMinFilter = crGL4TranslateTextureFilter(Command.ShaderTextureBindings[BindingIndex].BindSettings.FilterSettings.MinFilter);
		GLint DesiredMagFilter = crGL4TranslateTextureFilter(Command.ShaderTextureBindings[BindingIndex].BindSettings.FilterSettings.MagFilter);

		if (crGL4Information.DirectStateAccessEnabled)
			{
			glBindTextureUnit(TextureBindingIndex, TextureInformation->OpenGLID);
			if (TextureInformation->GLSWrap != DesiredSWrap)
				glTextureParameteri(TextureInformation->OpenGLID, GL_TEXTURE_WRAP_S, DesiredSWrap);
			if (TextureInformation->GLTWrap != DesiredTWrap)
				glTextureParameteri(TextureInformation->OpenGLID, GL_TEXTURE_WRAP_T, DesiredTWrap);
			if (TextureInformation->GLMinFilter != DesiredMinFilter)
				glTextureParameteri(TextureInformation->OpenGLID, GL_TEXTURE_MIN_FILTER, DesiredMinFilter);
			if (TextureInformation->GLMagFilter != DesiredMagFilter)
				glTextureParameteri(TextureInformation->OpenGLID, GL_TEXTURE_MAG_FILTER, DesiredMagFilter);
			glProgramUniform1i(ShaderInformation->OpenGLID, UniformInformation->OpenGLID, (GLint)TextureBindingIndex);
			}
		else
			{
			glActiveTexture(GL_TEXTURE0 + TextureBindingIndex); // Multitexture index
			glBindTexture(TextureInformation->GLTextureType, TextureInformation->OpenGLID);
			if (TextureInformation->GLSWrap != DesiredSWrap)
				glTexParameteri(TextureInformation->GLTextureType, GL_TEXTURE_WRAP_S, DesiredSWrap);
			if (TextureInformation->GLTWrap != DesiredTWrap)
				glTexParameteri(TextureInformation->GLTextureType, GL_TEXTURE_WRAP_T, DesiredTWrap);
			if (TextureInformation->GLMinFilter != DesiredMinFilter)
				glTexParameteri(TextureInformation->GLTextureType, GL_TEXTURE_MIN_FILTER, DesiredMinFilter);
			if (TextureInformation->GLMagFilter != DesiredMagFilter)
				glTexParameteri(TextureInformation->GLTextureType, GL_TEXTURE_MAG_FILTER, DesiredMagFilter);
			glUniform1i(UniformInformation->OpenGLID, (GLint)TextureBindingIndex);
			}

		if (crGL4CheckError() == false)
			{
			LOG_ERROR("Error activating texture");
			return false;
			}
		TextureInformation->GLSWrap = DesiredSWrap;
		TextureInformation->GLTWrap = DesiredTWrap;
		TextureInformation->GLMinFilter = DesiredMinFilter;
		TextureInformation->GLMagFilter = DesiredMagFilter;

		++TextureBindingIndex;
		}
	if (crGL4CheckError() == false)
		return false;

	// Finally, issue the draw call
	if (Command.IndexBufferStream.BufferHandle)
		{
		static size_t Sizes[] = { sizeof(float), sizeof(uint8_t), sizeof(uint16_t), sizeof(uint32_t) };

		crGL4InternalShaderBufferInfo *ShaderBufferInformation = (crGL4InternalShaderBufferInfo *)PointerList_GetNodeData(Command.IndexBufferStream.BufferHandle);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ShaderBufferInformation->OpenGLID);
		glDrawElementsInstanced(crGL4TranslatePrimitive(Command.Primitive),
		                        (GLsizei)Command.VertexCount,
		                        crGL4TranslateShaderBufferComponentType(Command.IndexBufferStream.ComponentType),
		                        (void *)(Command.IndexBufferStream.StartOffset + Command.StartVertex * Sizes[(int)Command.IndexBufferStream.ComponentType]),
		                        (GLsizei)Command.InstanceCount);
		}
	else
		{
		glDrawArraysInstanced(crGL4TranslatePrimitive(Command.Primitive),
		                      (GLint)Command.StartVertex,
		                      (GLsizei)Command.VertexCount,
		                      (GLsizei)Command.InstanceCount);
		}
	return crGL4CheckError();
	}
