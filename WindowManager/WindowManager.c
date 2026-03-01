#include <Platform/Platform.h>
#include "WindowManager.h"
#include "Windows/WindowsBackend.h"

crWindowHandle(*crCreateNewWindow) (const crRenderWindowDescriptor Descriptor) = NULL;
bool (*crDestroyWindow) (const crWindowHandle WindowHandle) = NULL;
bool (*crUpdateWindows) (const bool Wait) = NULL;
bool (*crSetWindowPosition) (const crWindowHandle WindowHandle, const ivec2 Position) = NULL;
bool (*crGetWindowPosition) (const crWindowHandle WindowHandle, ivec2 *Position) = NULL;
bool (*crSetWindowDimensions) (const crWindowHandle WindowHandle, const uvec2 Dimensions) = NULL;
bool (*crGetWindowDimensions) (const crWindowHandle WindowHandle, uvec2 *Dimensions) = NULL;
bool (*crGetWindowClientAreaDimensions) (const crWindowHandle WindowHandle, uvec2 *Dimensions) = NULL;
bool (*crSetWindowTitle) (const crWindowHandle WindowHandle, const char *Title) = NULL;
const char *(*crGetWindowTitle) (const crWindowHandle WindowHandle) = NULL;
bool (*crActivateWindow) (const crWindowHandle WindowHandle) = NULL;

bool SetupWindowManagerFunctionPointers(const crWindowManagerBackend Backend)
{
	switch (Backend)
	{
#if defined (PLATFORM_WINDOWS)
		case WindowManagerBackend_Windows:
		{
#define ASSIGN_POINTER(NAME) cr##NAME = crWindows##NAME
			ASSIGN_POINTER(CreateNewWindow);
			ASSIGN_POINTER(DestroyWindow);
			ASSIGN_POINTER(UpdateWindows);
			ASSIGN_POINTER(SetWindowPosition);
			ASSIGN_POINTER(GetWindowPosition);
			ASSIGN_POINTER(SetWindowDimensions);
			ASSIGN_POINTER(GetWindowDimensions);
			ASSIGN_POINTER(GetWindowClientAreaDimensions);
			ASSIGN_POINTER(SetWindowTitle);
			ASSIGN_POINTER(GetWindowTitle);
			ASSIGN_POINTER(ActivateWindow);
#undef ASSIGN_POINTER
			break;
		}
#endif
	}
	return true;
}