#pragma once
#include <stdbool.h>
#include <Platform/defines.h>
#include <Math/mathlib.h>
#include "KeyDefinitions.h"

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

typedef enum
	{
	WindowManagerBackend_Windows,
    WindowManagerBackend_X11
	} crWindowManagerBackend;

// Callbacks
typedef void ( *crWindowClosedCallback ) ( const crWindowHandle Handle );
typedef void ( *crWindowMovedCallback ) ( const crWindowHandle Handle, const ivec2 NewPosition );
typedef void ( *crWindowResizedCallback ) ( const crWindowHandle Handle, const uvec2 NewSize );
typedef void ( *crEndWindowResizedCallback ) ( const crWindowHandle Handle, const uvec2 NewSize );
typedef void ( *crWindowFocusChangedCallback ) ( const crWindowHandle Handle, const bool HasFocus );

typedef void ( *crMouseButtonCallback ) ( const crWindowHandle, const unsigned Button, const bool Clicked );
typedef void ( *crMouseMovedCallback ) ( const crWindowHandle, const ivec2 Delta, const ivec2 NewPosition );
typedef void ( *crMouseWheelCallback ) ( const crWindowHandle, const int Delta );

typedef void ( *crKeyStateChangedCallback ) ( const crWindowHandle, const crKeyCode Key, const bool State );

typedef struct
	{
	crWindowClosedCallback WindowClosed;
	crWindowMovedCallback WindowMoved;
	crWindowResizedCallback WindowResized;
	crEndWindowResizedCallback EndWindowResized;
	crWindowFocusChangedCallback WindowFocusChanged;
	crKeyStateChangedCallback KeyStateChanged;
	crMouseButtonCallback MouseButton;
	crMouseMovedCallback MouseMoved;
	crMouseWheelCallback MouseWheel;
	} crWindowManagerCallbacks;

// Function pointers
extern crWindowHandle (*crCreateNewWindow) ( const crRenderWindowDescriptor Descriptor );
extern bool (*crDestroyWindow) ( const crWindowHandle WindowHandle );
extern bool (*crUpdateWindows) ( const bool Wait );
extern bool (*crSetWindowPosition) ( const crWindowHandle WindowHandle, const ivec2 Position );
extern bool (*crGetWindowPosition) ( const crWindowHandle WindowHandle, ivec2 *Position );
extern bool (*crSetWindowDimensions) ( const crWindowHandle WindowHandle, const uvec2 Dimensions );
extern bool (*crGetWindowDimensions) ( const crWindowHandle WindowHandle, uvec2 *Dimensions );
extern bool (*crGetWindowClientAreaDimensions) ( const crWindowHandle WindowHandle, uvec2 *Dimensions );
extern bool (*crSetWindowTitle) ( const crWindowHandle WindowHandle, const char *Title );
extern const char *(*crGetWindowTitle) ( const crWindowHandle WindowHandle );
extern bool (*crActivateWindow) ( const crWindowHandle WindowHandle );

bool SetupWindowManagerFunctionPointers ( const crWindowManagerBackend Backend );
END_C_DECLARATIONS
