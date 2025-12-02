#pragma once

BEGIN_C_DECLARATIONS
typedef enum
	{
	crPixelFormat_Red8 = 0,
	crPixelFormat_RedGreen88,
	crPixelFormat_RedGreenBlue888,
	crPixelFormat_RedGreenBlueAlpha8888,
	crPixelFormat_AlphaRedGreenBlue8888,
	crPixelFormat_RedGreenBlue565,
	crPixelFormat_DepthComponent,
	} crPixelFormat;
END_C_DECLARATIONS
