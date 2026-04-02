#include "PixelFormat.h"

int BytesPerPixel ( const crPixelFormat Format )
	{
	switch ( Format )
		{
		case crPixelFormat_Red8:
			return 1;
		case crPixelFormat_RedGreenBlue565:
		case crPixelFormat_RedGreen88:
			return 2;
		case crPixelFormat_RedGreenBlue888:
			return 3;
		case crPixelFormat_RedGreenBlueAlpha8888:
		case crPixelFormat_AlphaRedGreenBlue8888:
		case crPixelFormat_DepthComponent:
			return 4;
		}
	return 0;
	}
