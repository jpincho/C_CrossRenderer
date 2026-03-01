#include "X11Backend.h"
#include "../Internal/WindowManagerInternal.h"
#include "X11Internal.h"
#include <Platform/Logger.h>
#include <C_Utils/PointerList.h>
#include <string.h>

static PointerListNode *GetPointerListNodeFromcrWindowHandle ( const crWindowHandle WindowHandle )
        {
        PointerListNode *Node = PointerList_GetFirst ( &WindowList );
        while ( Node != NULL )
                {
                struct InternalX11WindowData *NodeData = ( struct InternalX11WindowData * ) PointerList_GetNodeData ( Node );
                if ( NodeData->WindowHandle == WindowHandle )
                        {
                        return Node;
                        }
                }
        return NULL;
        }

static PointerListNode *GetPointerListNodeFromX11WindowHandle ( const Window WindowHandle )
        {
        PointerListNode *Node = PointerList_GetFirst ( &WindowList );
        while ( Node != NULL )
                {
                struct InternalX11WindowData *NodeData = ( struct InternalX11WindowData * ) PointerList_GetNodeData ( Node );
                if ( NodeData->X11WindowHandle == WindowHandle )
                        {
                        return Node;
                        }
                }
        return NULL;
        }

struct InternalX11WindowData *GetInternalX11WindowDataFromX11WindowHandle ( const Window WindowHandle )
        {
        PointerListNode *Node = PointerList_GetFirst ( &WindowList );
        while ( Node != NULL )
                {
                struct InternalX11WindowData *NodeData = ( struct InternalX11WindowData * ) PointerList_GetNodeData ( Node );
                if ( NodeData->X11WindowHandle == WindowHandle )
                        {
                        return NodeData;
                        }
                }
        return NULL;
        }

struct InternalX11WindowData *GetInternalX11WindowDataFromcrWindowHandle ( const crWindowHandle WindowHandle )
        {
        PointerListNode *Node = PointerList_GetFirst ( &WindowList );
        while ( Node != NULL )
                {
                struct InternalX11WindowData *NodeData = ( struct InternalX11WindowData * ) PointerList_GetNodeData ( Node );
                if ( NodeData->WindowHandle == WindowHandle )
                        {
                        return NodeData;
                        }
                }
        return NULL;
        }

static int X11ErrorHandler ( Display *dpy, XErrorEvent *e )
        {
        char errorText[1024];
        XGetErrorText ( dpy, e->error_code, errorText, sizeof ( errorText ) );
        LOG_ERROR ( "**********************************\n" );
        LOG_ERROR ( "X Error: %s\n", errorText );
        LOG_ERROR ( "**********************************\n" );

        // exit ( 1 );
        return -1;
        }

bool crX11InitializeWindowBackend ( void )
	{
	return true;
	}

crWindowHandle crX11CreateNewWindow ( const crRenderWindowDescriptor Descriptor )
        {
        // To create a new window, I need a Display, a screen, and a visual. Here I setup all of those
        XSetErrorHandler ( X11ErrorHandler );
        Display *CurrentDisplay = XOpenDisplay ( 0 );
        int CurrentScreenID = DefaultScreen ( CurrentDisplay );

        int BorderWidth = 0;
        int WindowDepth = CopyFromParent;
        int WindowClass = CopyFromParent;
        Visual *WindowVisual = CopyFromParent;

        int AttributeValueMask = CWBackPixel | CWEventMask;
        XSetWindowAttributes WindowAttributes = {0};
        WindowAttributes.background_pixel = 0;
        WindowAttributes.event_mask = ExposureMask | StructureNotifyMask | KeyPressMask | ButtonPressMask;
        WindowAttributes.border_pixel = 0;

        Window NewX11WindowHandle = XCreateWindow ( CurrentDisplay,
                                    RootWindow ( CurrentDisplay, CurrentScreenID ),
                                    Descriptor.Position.x, Descriptor.Position.y,
                                    Descriptor.Size.x, Descriptor.Size.y,
                                    BorderWidth,
                                    WindowDepth,
                                    WindowClass,
                                    WindowVisual,
                                    AttributeValueMask,
                                    &WindowAttributes );

        struct InternalX11WindowData *NewWindowData = calloc ( 1, sizeof ( struct InternalX11WindowData ) );
        NewWindowData->WindowHandle = NewWindowData;
        NewWindowData->X11WindowHandle = NewX11WindowHandle;
        NewWindowData->DisplayHandle = CurrentDisplay;
        NewWindowData->Dimensions.x = Descriptor.Size.x;
        NewWindowData->Dimensions.y = Descriptor.Size.y;
        NewWindowData->Position.x = Descriptor.Position.x;
        NewWindowData->Position.y = Descriptor.Position.y;
        NewWindowData->ScreenID = CurrentScreenID;

        LOG_DEBUG ( "Finished creating window %dx%d with size %ux%u. Title '%s'",
                    Descriptor.Position.x,
                    Descriptor.Position.y,
                    Descriptor.Size.x,
                    Descriptor.Size.y,
                    Descriptor.Title );

        XMapWindow ( NewWindowData->DisplayHandle, NewWindowData->X11WindowHandle );
        XFlush ( NewWindowData->DisplayHandle );
        XSync ( NewWindowData->DisplayHandle, false );
        XUngrabServer ( NewWindowData->DisplayHandle );

        NewWindowData->Atoms.WM_DELETE_WINDOW = XInternAtom ( NewWindowData->DisplayHandle, "WM_DELETE_WINDOW", False );
        if ( !XSetWMProtocols ( NewWindowData->DisplayHandle, NewWindowData->X11WindowHandle, &NewWindowData->Atoms.WM_DELETE_WINDOW, 1 ) )
                {
                XDestroyWindow ( NewWindowData->DisplayHandle, NewWindowData->X11WindowHandle );
                free ( NewWindowData );
                return NULL;
                }

        PointerList_AddAtEnd ( &WindowList, ( void * ) NewWindowData );
        crSetWindowTitle ( NewWindowData, Descriptor.Title );
        return ( crWindowHandle ) NewWindowData;
        }

bool crX11DestroyWindow ( const crWindowHandle WindowHandle )
	{
        PointerListNode *Node = PointerList_Find ( &WindowList, NULL, WindowHandle );
        if ( Node == NULL )
                {
                return false;
                }
        struct InternalX11WindowData *WindowData = ( struct InternalX11WindowData * ) PointerList_GetNodeData ( Node );
        XDestroyWindow ( WindowData->DisplayHandle, WindowData->X11WindowHandle );
        free ( WindowData );
        PointerList_DestroyNode ( &WindowList, Node );
        return true;
        }


static void ProcessXEvent ( const XEvent Event )
        {
        struct InternalX11WindowData *WindowData = GetInternalX11WindowDataFromX11WindowHandle ( Event.xclient.window );
        if ( WindowData == NULL )
                return;

        switch ( Event.type )
                {
                case ConfigureNotify:
                        {
                        if ( ( ( unsigned ) Event.xconfigure.width != WindowData->Dimensions.x ) || ( ( unsigned ) Event.xconfigure.height != WindowData->Dimensions.y ) )
                                {
                                if ( WindowManagerCallbacks.WindowResized )
                                        {
                                        WindowData->Dimensions.x = Event.xconfigure.width;
                                        WindowData->Dimensions.y = Event.xconfigure.height;
                                        WindowManagerCallbacks.WindowResized ( WindowData->WindowHandle, WindowData->Dimensions );
                                        }
                                }
                        if ( ( Event.xconfigure.x != WindowData->Position.x ) || ( Event.xconfigure.y != WindowData->Position.y ) )
                                {
                                if ( WindowManagerCallbacks.WindowMoved )
                                        {
                                        WindowData->Position.x = Event.xconfigure.x;
                                        WindowData->Position.y = Event.xconfigure.y;
                                        WindowManagerCallbacks.WindowMoved ( WindowData->WindowHandle, WindowData->Position );
                                        }
                                }
                        break;
                        }

                case KeyPress:
                case KeyRelease:
                        {
                        /*
                        XKeyPressedEvent *KeyPressedEvent = ( XKeyPressedEvent * ) &Event;
                        if(Event->keycode == XKeysymToKeycode(MainDisplay, XK_Escape))
                        {
                        IsWindowOpen = 0;
                        }

                        if(Event->keycode == XKeysymToKeycode(MainDisplay, XK_Down))
                        {
                        Box.Y += StepSize;
                        }
                        else if(Event->keycode == XKeysymToKeycode(MainDisplay, XK_Up))
                        {
                        Box.Y -= StepSize;
                        }
                        else if(Event->keycode == XKeysymToKeycode(MainDisplay,
                        XK_Right))
                        {
                        Box.X += StepSize;
                        }
                        else if(Event->keycode == XKeysymToKeycode(MainDisplay,
                        XK_Left))
                        {
                        Box.X -= StepSize;
                        }*/
                        break;
                        }

                case ClientMessage:
                        {
                        XClientMessageEvent *ClientMessageEvent = ( XClientMessageEvent * ) &Event;
                        PointerListNode *Node = GetPointerListNodeFromcrWindowHandle ( WindowData->WindowHandle );
                        if ( Node == NULL )
                                break;
                        if ( ( Atom ) ClientMessageEvent->data.l[0] == WindowData->Atoms.WM_DELETE_WINDOW )
                                {
                                if ( WindowManagerCallbacks.WindowClosed )
                                        WindowManagerCallbacks.WindowClosed ( WindowData->WindowHandle );

                                XDestroyWindow ( WindowData->DisplayHandle, WindowData->X11WindowHandle );
                                PointerList_DestroyNode ( &WindowList, Node );
                                }
                        break;
                        }
                }
        }

bool crX11UpdateWindows ( const bool Wait )
        {
        Display *CurrentDisplay = XOpenDisplay ( 0 );
        XEvent Event = {};
        PointerListNode *Node = PointerList_GetFirst ( &WindowList );
        while ( Node != NULL )
                {
                struct InternalX11WindowData *X11WindowData = ( struct InternalX11WindowData * ) PointerList_GetNodeData ( Node );
                CurrentDisplay = X11WindowData->DisplayHandle;

                while ( XPending ( CurrentDisplay ) )
                        {
                        XNextEvent ( CurrentDisplay, &Event );
                        ProcessXEvent ( Event );
                        }
                Node = PointerList_GetNextNode ( Node );
                }
        if ( ( Wait ) && ( CurrentDisplay != NULL ) )
                {
                XNextEvent ( CurrentDisplay, &Event );
                ProcessXEvent ( Event );
                }

        return true;
        }

bool crX11SetWindowPosition ( const crWindowHandle WindowHandle, const ivec2 Position )
        {
        struct InternalX11WindowData *WindowData = GetInternalX11WindowDataFromcrWindowHandle ( WindowHandle );
        if ( WindowData == NULL )
                return false;
        WindowData->Position.x = Position.x;
        WindowData->Position.y = Position.y;
        XMoveResizeWindow ( WindowData->DisplayHandle,
                            WindowData->X11WindowHandle,
                            WindowData->Position.x,
                            WindowData->Position.y,
                            WindowData->Dimensions.x,
                            WindowData->Dimensions.y );
        return true;
        }

bool crX11GetWindowPosition ( const crWindowHandle WindowHandle, ivec2 *Position )
	{
        struct InternalX11WindowData *WindowData = GetInternalX11WindowDataFromcrWindowHandle ( WindowHandle );
        if ( WindowData == NULL )
                return false;
        *Position = WindowData->Position;
        return true;
        }

bool crX11SetWindowDimensions ( const crWindowHandle WindowHandle, const uvec2 Dimensions )
	{
        struct InternalX11WindowData *WindowData = GetInternalX11WindowDataFromcrWindowHandle ( WindowHandle );
        if ( WindowData == NULL )
                return false;

        WindowData->Dimensions.x = Dimensions.x;
        WindowData->Dimensions.y = Dimensions.y;
        XMoveResizeWindow ( WindowData->DisplayHandle,
                            WindowData->X11WindowHandle,
                            WindowData->Position.x,
                            WindowData->Position.y,
                            WindowData->Dimensions.x,
                            WindowData->Dimensions.y );
        return true;
        }

bool crX11GetWindowDimensions ( const crWindowHandle WindowHandle, uvec2 *Dimensions )
        {
        struct InternalX11WindowData *WindowData = GetInternalX11WindowDataFromcrWindowHandle ( WindowHandle );
        if ( WindowData == NULL )
                return false;
        *Dimensions = WindowData->Dimensions;
        return true;
        }

bool crX11GetWindowClientAreaDimensions ( const crWindowHandle WindowHandle, uvec2 *Dimensions )
	{
        return crGetWindowDimensions ( WindowHandle, Dimensions );
	}

bool crX11SetWindowTitle ( const crWindowHandle WindowHandle, const char *Title )
	{
        struct InternalX11WindowData *WindowData = GetInternalX11WindowDataFromcrWindowHandle ( WindowHandle );
        if ( WindowData == NULL )
                return false;
        void *TempPointer = realloc ( WindowData->Title, strlen ( Title ) + 1 );
        if ( !TempPointer )
                return false;
        WindowData->Title = TempPointer;
        strcpy ( WindowData->Title, Title );
        return true;
	}

const char *crX11GetWindowTitle ( const crWindowHandle WindowHandle )
	{
        struct InternalX11WindowData *WindowData = GetInternalX11WindowDataFromcrWindowHandle ( WindowHandle );
        if ( WindowData == NULL )
                return false;
        return WindowData->Title;
        }

bool crX11ActivateWindow ( const crWindowHandle WindowHandle )
	{
        struct InternalX11WindowData *WindowData = GetInternalX11WindowDataFromcrWindowHandle ( WindowHandle );
        if ( WindowData == NULL )
                return false;
        XMapWindow ( WindowData->DisplayHandle, WindowData->X11WindowHandle );
        return true;
        }
