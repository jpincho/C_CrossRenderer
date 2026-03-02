#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "WindowManager/WindowManager.h"

BEGIN_C_DECLARATIONS
typedef enum
	{
	OpenGLCore,
	} crRendererBackend;

typedef struct
	{
	crRenderWindowDescriptor InitialWindowDescriptor;
	crRendererBackend DesiredRendererBackend;
	crWindowManagerBackend DesiredWindowManagerBackend;
	bool VSyncEnabled;
	uint8_t RedBits, GreenBits, BlueBits, AlphaBits, DepthBits, StencilBits;
	} crRendererConfiguration;
END_C_DECLARATIONS
