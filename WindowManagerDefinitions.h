#pragma once
#include <Math/mathlib.h>
#include <stdbool.h>
#include <Platform/defines.h>

BEGIN_C_DECLARATIONS

typedef void *crWindowHandle;

struct crRenderWindowDescriptor
	{
	ivec2 Size;
	ivec2 Position;
	bool Resizable, Fullscreen;
	bool SupportOpenGL;
	const char *Title;
	};

typedef void ( *crWindowClosedCallback )( const crWindowHandle Handle );
typedef void ( *crWindowMovedCallback )( const crWindowHandle Handle, const ivec2 NewPosition );
typedef void ( *crWindowResizedCallback )( const crWindowHandle Handle, const ivec2 NewSize );
typedef void ( *crEndWindowResizedCallback )( const crWindowHandle Handle, const ivec2 NewSize );
typedef void ( *crWindowFocusChangedCallback )( const crWindowHandle Handle, const bool HasFocus );

typedef void ( *MouseButtonCallback )( const crWindowHandle, const int Button, const bool Clicked );
typedef void ( *MouseMovedCallback )( const crWindowHandle, const ivec2 Delta, const ivec2 NewPosition );
typedef void ( *MouseWheelCallback ) ( const crWindowHandle, const int Delta );

typedef struct
	{
	crWindowClosedCallback WindowClosed;
	crWindowMovedCallback WindowMoved;
	crWindowResizedCallback WindowResized;
	crEndWindowResizedCallback EndWindowResized;
	crWindowFocusChangedCallback WindowFocusChanged;
	MouseButtonCallback MouseButton;
	MouseMovedCallback MouseMoved;
	MouseWheelCallback MouseWheel;
	}crWindowManagerCallbacks;

END_C_DECLARATIONS
