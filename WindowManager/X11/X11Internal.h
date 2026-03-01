#pragma once
#include <X11/Xlib.h>
#include "../WindowManager.h"

struct InternalX11WindowData
{
    crWindowHandle WindowHandle;
    uvec2 Dimensions;
    ivec2 Position;
    char *Title;
    Display *DisplayHandle;
    int ScreenID;
    Window X11WindowHandle;
    //GLXContext Context;
    struct
    {
        Atom WM_DELETE_WINDOW;
    } Atoms;
};

struct InternalX11WindowData *GetInternalX11WindowDataFromX11WindowHandle ( const Window WindowHandle );
struct InternalX11WindowData *GetInternalX11WindowDataFromcrWindowHandle ( const crWindowHandle WindowHandle );
