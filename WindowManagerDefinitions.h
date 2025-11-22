#pragma once
#include <Math/mathlib.h>
#include <stdbool.h>
#include <Platform/defines.h>

BEGIN_C_DECLARATIONS

typedef void *crWindowHandle;

typedef struct
	{
	ivec2 Size;
	ivec2 Position;
	bool Resizable, Fullscreen;
	bool SupportOpenGL;
	const char *Title;
	} crRenderWindowDescriptor;

typedef void ( *crWindowClosedCallback )( const crWindowHandle Handle );
typedef void ( *crWindowMovedCallback )( const crWindowHandle Handle, const ivec2 NewPosition );
typedef void ( *crWindowResizedCallback )( const crWindowHandle Handle, const uvec2 NewSize );
typedef void ( *crEndWindowResizedCallback )( const crWindowHandle Handle, const uvec2 NewSize );
typedef void ( *crWindowFocusChangedCallback )( const crWindowHandle Handle, const bool HasFocus );

typedef void ( *crMouseButtonCallback )( const crWindowHandle, const unsigned Button, const bool Clicked );
typedef void ( *crMouseMovedCallback )( const crWindowHandle, const ivec2 Delta, const ivec2 NewPosition );
typedef void ( *crMouseWheelCallback ) ( const crWindowHandle, const int Delta );

typedef void ( *crKeyDownCallback )( const crWindowHandle, const unsigned Key );
typedef void ( *crKeyUpCallback )( const crWindowHandle, const unsigned Key );

typedef struct
	{
	crWindowClosedCallback WindowClosed;
	crWindowMovedCallback WindowMoved;
	crWindowResizedCallback WindowResized;
	crEndWindowResizedCallback EndWindowResized;
	crWindowFocusChangedCallback WindowFocusChanged;
	crKeyDownCallback KeyDown;
	crKeyUpCallback KeyUp;
	crMouseButtonCallback MouseButton;
	crMouseMovedCallback MouseMoved;
	crMouseWheelCallback MouseWheel;
	} crWindowManagerCallbacks;

END_C_DECLARATIONS
