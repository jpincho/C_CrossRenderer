#include <CrossRenderer.h>
#include <Platform/Logger.h>

static bool ShouldQuit = false;
void WindowClosed_Callback(const crWindowHandle Handle)
    {
    ShouldQuit = true;
    }

void WindowMoved_Callback(const crWindowHandle Handle, const ivec2 NewPosition)
    {
    LOG_DEBUG("Window moved to %d %d", NewPosition.x, NewPosition.y);
    }

void WindowResized_Callback(const crWindowHandle Handle, const ivec2 NewSize)
    {
    LOG_DEBUG("Window resized to %d %d", NewSize.x, NewSize.y);
    }

void MouseClicked_Callback(const crWindowHandle Handle, const int Button, const bool Click)
    {
    LOG_DEBUG("Mouse button %u %s", Button, Click ? "clicked" : "released");
    }

void MouseWheel_Callback(const crWindowHandle Handle, const int Delta)
    {
    LOG_DEBUG("Mouse wheel %d", Delta);
    }

void MouseMoved_Callback(const crWindowHandle Handle, const ivec2 Delta, const ivec2 NewPosition)
    {
    LOG_DEBUG("Mouse moved %d %d %d %d", Delta.x, Delta.y, NewPosition.x, NewPosition.y);
    }

void WindowFocusChanged_Callback(const crWindowHandle Handle, const bool HasFocus)
    {
    LOG_DEBUG("Window focus %s", HasFocus ? "true" : "false");
    }

int main(int argc, char *argv[])
    {
    crRendererConfiguration Configuration = { 0 };
    Configuration.InitialWindowDescriptor.Fullscreen = false;
    Configuration.InitialWindowDescriptor.Position.x = 100;
    Configuration.InitialWindowDescriptor.Position.y = 100;
    Configuration.InitialWindowDescriptor.Size.x = 1024;
    Configuration.InitialWindowDescriptor.Size.y = 768;
    Configuration.InitialWindowDescriptor.Resizable = true;
    Configuration.InitialWindowDescriptor.Title = "Test window";
    Configuration.VSyncEnabled = true;
    if (crInitialize(Configuration) == false)
        return -1;

    crWindowManagerCallbacks Callbacks;
    memset(&Callbacks, 0, sizeof(Callbacks));
    Callbacks.EndWindowResized = WindowResized_Callback;
    Callbacks.WindowClosed = WindowClosed_Callback;
    Callbacks.WindowMoved = WindowMoved_Callback;
    Callbacks.MouseButton = MouseClicked_Callback;
    Callbacks.MouseWheel = MouseWheel_Callback;
    Callbacks.MouseMoved = MouseMoved_Callback;
    Callbacks.WindowFocusChanged = WindowFocusChanged_Callback;
    crSetWindowManagerCallbacks(Callbacks);

    while (ShouldQuit == false)
        {

        crUpdateWindows(true);
        }
    return 0;
    }