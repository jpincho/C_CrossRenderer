#pragma once
#include "../WindowManager.h"

BEGIN_C_DECLARATIONS
bool crX11InitializeWindowBackend ( void );
crWindowHandle crX11CreateNewWindow ( const crRenderWindowDescriptor Descriptor );
bool crX11DestroyWindow ( const crWindowHandle WindowHandle );
bool crX11UpdateWindows ( const bool Wait );
bool crX11SetWindowPosition ( const crWindowHandle WindowHandle, const ivec2 Position );
bool crX11GetWindowPosition ( const crWindowHandle WindowHandle, ivec2 *Position );
bool crX11SetWindowDimensions ( const crWindowHandle WindowHandle, const uvec2 Dimensions );
bool crX11GetWindowDimensions ( const crWindowHandle WindowHandle, uvec2 *Dimensions );
bool crX11GetWindowClientAreaDimensions ( const crWindowHandle WindowHandle, uvec2 *Dimensions );
bool crX11SetWindowTitle ( const crWindowHandle WindowHandle, const char *Title );
const char *crX11GetWindowTitle ( const crWindowHandle WindowHandle );
bool crX11ActivateWindow ( const crWindowHandle WindowHandle );

END_C_DECLARATIONS
