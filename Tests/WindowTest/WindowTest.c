#include <CrossRenderer.h>
#include <Platform/Logger.h>

static bool ShouldQuit = false;
void WindowClosed_Callback(const crWindowHandle Handle)
	{
	UNUSED(Handle);
	ShouldQuit = true;
	}

void WindowMoved_Callback(const crWindowHandle Handle, const ivec2 NewPosition)
	{
	UNUSED(Handle);
	LOG_DEBUG("Window moved to %d %d", NewPosition.x, NewPosition.y);
	}

void WindowResized_Callback(const crWindowHandle Handle, const uvec2 NewSize)
	{
	UNUSED(Handle);
	LOG_DEBUG("Window resized to %u %u", NewSize.x, NewSize.y);
	}

void MouseClicked_Callback(const crWindowHandle Handle, const unsigned Button, const bool Click)
	{
	UNUSED(Handle);
	LOG_DEBUG("Mouse button %u %s", Button, Click ? "clicked" : "released");
	}

void MouseWheel_Callback(const crWindowHandle Handle, const int Delta)
	{
	UNUSED(Handle);
	LOG_DEBUG("Mouse wheel %d", Delta);
	}

void MouseMoved_Callback(const crWindowHandle Handle, const ivec2 Delta, const ivec2 NewPosition)
	{
	UNUSED(Handle);
	LOG_DEBUG("Mouse moved %d %d %d %d", Delta.x, Delta.y, NewPosition.x, NewPosition.y);
	}

void WindowFocusChanged_Callback(const crWindowHandle Handle, const bool HasFocus)
	{
	UNUSED(Handle);
	LOG_DEBUG("Window focus %s", HasFocus ? "true" : "false");
	}

int main(void)
	{
	crRenderWindowDescriptor NewWindowDescriptor;
	NewWindowDescriptor.Fullscreen = false;
	NewWindowDescriptor.Position.x = 100;
	NewWindowDescriptor.Position.y = 100;
	NewWindowDescriptor.Size.x = 1024;
	NewWindowDescriptor.Size.y = 768;
	NewWindowDescriptor.Resizable = true;
	NewWindowDescriptor.Title = "Test window";
	crWindowHandle WindowHandle = crCreateNewWindow(NewWindowDescriptor);
	if (!WindowHandle)
		return -1;

	crWindowManagerCallbacks Callbacks = { 0 };
	Callbacks.EndWindowResized = WindowResized_Callback;
	Callbacks.WindowClosed = WindowClosed_Callback;
	Callbacks.WindowMoved = WindowMoved_Callback;
	Callbacks.MouseButton = MouseClicked_Callback;
	Callbacks.MouseWheel = MouseWheel_Callback;
	Callbacks.MouseMoved = MouseMoved_Callback;
	Callbacks.WindowFocusChanged = WindowFocusChanged_Callback;
	crSetWindowManagerCallbacks(Callbacks);

	while (ShouldQuit == false) crUpdateWindows(true);
	return 0;
	}