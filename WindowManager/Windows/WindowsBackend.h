#pragma once
#include "../WindowManager.h"

BEGIN_C_DECLARATIONS
bool crWindowsInitializeWindowBackend ( void );
crWindowHandle crWindowsCreateNewWindow ( const crRenderWindowDescriptor Descriptor );
bool crWindowsDestroyWindow ( const crWindowHandle WindowHandle );
bool crWindowsUpdateWindows ( const bool Wait );
bool crWindowsSetWindowPosition ( const crWindowHandle WindowHandle, const ivec2 Position );
bool crWindowsGetWindowPosition ( const crWindowHandle WindowHandle, ivec2 *Position );
bool crWindowsSetWindowDimensions ( const crWindowHandle WindowHandle, const uvec2 Dimensions );
bool crWindowsGetWindowDimensions ( const crWindowHandle WindowHandle, uvec2 *Dimensions );
bool crWindowsGetWindowClientAreaDimensions ( const crWindowHandle WindowHandle, uvec2 *Dimensions );
bool crWindowsSetWindowTitle ( const crWindowHandle WindowHandle, const char *Title );
const char *crWindowsGetWindowTitle ( const crWindowHandle WindowHandle );
bool crWindowsActivateWindow ( const crWindowHandle WindowHandle );
END_C_DECLARATIONS
