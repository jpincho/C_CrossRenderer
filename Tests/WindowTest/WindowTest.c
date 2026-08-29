#include <CrossRenderer.h>
#include <Platform/Logger.h>

static bool ShouldQuit = false;
void WindowClosed_Callback ( const crWindowHandle Handle )
    {
    UNUSED ( Handle );
    ShouldQuit = true;
    }

void WindowMoved_Callback ( const crWindowHandle Handle, const ivec2 NewPosition )
    {
    UNUSED ( Handle );
    LOG_DEBUG ( "Window moved to %d %d", NewPosition.x, NewPosition.y );
    }

void WindowResized_Callback ( const crWindowHandle Handle, const uvec2 NewSize )
    {
    UNUSED ( Handle );
    LOG_DEBUG ( "Window resized to %u %u", NewSize.x, NewSize.y );
    }

void MouseClicked_Callback ( const crWindowHandle Handle, const unsigned Button, const bool Click )
    {
    UNUSED ( Handle );
    LOG_DEBUG ( "Mouse button %u %s", Button, Click ? "clicked" : "released" );
    }

void MouseWheel_Callback ( const crWindowHandle Handle, const int Delta )
    {
    UNUSED ( Handle );
    LOG_DEBUG ( "Mouse wheel %d", Delta );
    }

void MouseMoved_Callback ( const crWindowHandle Handle, const ivec2 NewPosition )
    {
    UNUSED ( Handle );
    LOG_DEBUG ( "Mouse moved %d %d", NewPosition.x, NewPosition.y );
    }

void WindowFocusChanged_Callback ( const crWindowHandle Handle, const bool HasFocus )
    {
    UNUSED ( Handle );
    LOG_DEBUG ( "Window focus %s", HasFocus ? "true" : "false" );
    }

void KeyStateChanged_Callback ( const crWindowHandle Handle, const crKeyCode Key, const bool State )
    {
    UNUSED ( Handle );
    LOG_DEBUG ( "Key %s ( 0x%02X ) state %s", crStringifycrKeyCode ( Key ), Key, State ? "true" : "false" );
    }

int main ( void )
    {
    crRendererConfiguration Configuration;
    crSetConfigurationToDefault ( &Configuration );
    Configuration.InitialWindowDescriptor.Title = "CrossRenderer - window test";
    if ( crInitialize ( Configuration ) == false )
        return -1;

    crWindowManagerCallbacks Callbacks = { 0 };
    Callbacks.EndWindowResized = WindowResized_Callback;
    Callbacks.WindowClosed = WindowClosed_Callback;
    Callbacks.WindowMoved = WindowMoved_Callback;
    Callbacks.MouseButtonStateChanged = MouseClicked_Callback;
    Callbacks.MouseWheel = MouseWheel_Callback;
    Callbacks.MouseMoved = MouseMoved_Callback;
    Callbacks.WindowFocusChanged = WindowFocusChanged_Callback;
    Callbacks.KeyStateChanged = KeyStateChanged_Callback;
    crSetWindowManagerCallbacks ( Callbacks );

    while ( ShouldQuit == false ) crUpdateWindows ();
    return 0;
    }