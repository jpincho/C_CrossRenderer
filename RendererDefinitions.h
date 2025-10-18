#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "WindowManagerDefinitions.h"

typedef enum
    {
    OpenGLCore,
    } crRendererBackend;

typedef struct
    {
    crRenderWindowDescriptor InitialWindowDescriptor;
    crRendererBackend DesiredRendererBackend;
    bool VSyncEnabled;
    uint8_t RedBits, GreenBits, BlueBits, AlphaBits, DepthBits, StencilBits;
    } crRendererConfiguration;
