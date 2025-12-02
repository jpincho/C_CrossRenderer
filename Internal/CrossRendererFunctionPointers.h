#pragma once

#define DECLARE_INTERFACE_FUNCTION(return,name,...) extern return (*cr##name) ( __VA_ARGS__ )
#include "RendererTemplateSignatures.h"
#undef DECLARE_INTERFACE_FUNCTION
