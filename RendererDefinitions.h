#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "WindowManagerDefinitions.h"

enum crRendererBackend
	{
	OpenGLCore,
	};

struct crRendererConfiguration
	{
	struct crRenderWindowDescriptor InitialWindowDescriptor;
	enum crRendererBackend DesiredRendererBackend;
	bool VSyncEnabled;
	uint8_t RedBits, GreenBits, BlueBits, AlphaBits, DepthBits, StencilBits;
	};
