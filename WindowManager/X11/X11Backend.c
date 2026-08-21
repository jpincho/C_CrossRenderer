#include "X11Backend.h"
#include "X11Internal.h"
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>
#include "../Internal/WindowManagerInternal.h"
#include <Platform/Logger.h>
#include <Platform/PointerList.h>
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
typedef struct
	{
	crKeyCode Key;
	bool State;
	} KeyState;

static KeyState KeyStates[512] = { 0 };

crKeyCode crX11TranslateKeyCodeTocrKeyCode ( const unsigned X11KeyCode )
	{
	//static short KeyCodes[512] = { 0 };
	static bool KeyCodesInitialized = false;

	if ( KeyCodesInitialized == false )
		{
		Display *MainDisplay = XOpenDisplay ( 0 );
		for ( unsigned KeyCode = 0; KeyCode < 512; ++KeyCode )
			{
			KeyStates[KeyCode].Key = -1;
			KeyStates[KeyCode].State = false;
			}

		for ( unsigned KeyCode = 0; KeyCode < 10; ++KeyCode )
			KeyStates[XKeysymToKeycode ( MainDisplay, KeyCode + XK_0 )].Key = KeyCode + crKeyCode_0;

		for ( unsigned KeyCode = 0; KeyCode < 26; ++KeyCode )
			KeyStates[XKeysymToKeycode ( MainDisplay, KeyCode + XK_A )].Key = KeyCode + crKeyCode_A;

		KeyStates[XKeysymToKeycode ( MainDisplay, XK_apostrophe )].Key = crKeyCode_Apostrophe;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_backslash )].Key = crKeyCode_Backslash;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_comma )].Key = crKeyCode_Comma;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_equal )].Key = crKeyCode_Equal;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_grave )].Key = crKeyCode_GraveAccent;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_bracketleft )].Key = crKeyCode_LeftBracket;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_minus )].Key = crKeyCode_Minus;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_period )].Key = crKeyCode_Period;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_bracketright )].Key = crKeyCode_RightBracket;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_semicolon )].Key = crKeyCode_Semicolon;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_slash )].Key = crKeyCode_Slash;

		KeyStates[XKeysymToKeycode ( MainDisplay, XK_BackSpace )].Key = crKeyCode_Backspace;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Delete )].Key = crKeyCode_Delete;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_End )].Key = crKeyCode_End;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Return )].Key = crKeyCode_Enter;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Escape )].Key = crKeyCode_Escape;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Home )].Key = crKeyCode_Home;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Insert )].Key = crKeyCode_Insert;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Menu )].Key = crKeyCode_Menu;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Page_Down )].Key = crKeyCode_PageDown;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Page_Up )].Key = crKeyCode_PageUp;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Pause )].Key = crKeyCode_Pause;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_space )].Key = crKeyCode_Space;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Tab )].Key = crKeyCode_Tab;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Caps_Lock )].Key = crKeyCode_CapsLock;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Num_Lock )].Key = crKeyCode_NumLock;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Scroll_Lock )].Key = crKeyCode_ScrollLock;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F1 )].Key = crKeyCode_F1;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F2 )].Key = crKeyCode_F2;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F3 )].Key = crKeyCode_F3;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F4 )].Key = crKeyCode_F4;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F5 )].Key = crKeyCode_F5;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F6 )].Key = crKeyCode_F6;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F7 )].Key = crKeyCode_F7;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F8 )].Key = crKeyCode_F8;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F9 )].Key = crKeyCode_F9;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F10 )].Key = crKeyCode_F10;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F11 )].Key = crKeyCode_F11;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F12 )].Key = crKeyCode_F12;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F13 )].Key = crKeyCode_F13;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F14 )].Key = crKeyCode_F14;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F15 )].Key = crKeyCode_F15;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F16 )].Key = crKeyCode_F16;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F17 )].Key = crKeyCode_F17;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F18 )].Key = crKeyCode_F18;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F19 )].Key = crKeyCode_F19;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F20 )].Key = crKeyCode_F20;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F21 )].Key = crKeyCode_F21;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F22 )].Key = crKeyCode_F22;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F23 )].Key = crKeyCode_F23;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_F24 )].Key = crKeyCode_F24;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Alt_L )].Key = crKeyCode_LeftAlt;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Control_L )].Key = crKeyCode_LeftControl;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Shift_L )].Key = crKeyCode_LeftShift;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Super_L )].Key = crKeyCode_LeftSuper;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Print )].Key = crKeyCode_PrintScreen;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Alt_R )].Key = crKeyCode_RightAlt;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Control_R )].Key = crKeyCode_RightControl;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Shift_R )].Key = crKeyCode_RightShift;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Super_R )].Key = crKeyCode_RightSuper;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Down )].Key = crKeyCode_Down;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Left )].Key = crKeyCode_Left;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Right )].Key = crKeyCode_Right;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_Up )].Key = crKeyCode_Up;

		KeyStates[XKeysymToKeycode ( MainDisplay, XK_KP_0 )].Key = crKeyCode_KeyPad_0;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_KP_1 )].Key = crKeyCode_KeyPad_1;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_KP_2 )].Key = crKeyCode_KeyPad_2;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_KP_3 )].Key = crKeyCode_KeyPad_3;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_KP_4 )].Key = crKeyCode_KeyPad_4;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_KP_5 )].Key = crKeyCode_KeyPad_5;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_KP_6 )].Key = crKeyCode_KeyPad_6;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_KP_7 )].Key = crKeyCode_KeyPad_7;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_KP_8 )].Key = crKeyCode_KeyPad_8;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_KP_9 )].Key = crKeyCode_KeyPad_9;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_KP_Add )].Key = crKeyCode_KeyPad_Add;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_KP_Decimal )].Key = crKeyCode_KeyPad_Decimal;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_KP_Divide )].Key = crKeyCode_KeyPad_Divide;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_KP_Enter )].Key = crKeyCode_KeyPad_Enter;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_KP_Equal )].Key = crKeyCode_KeyPad_Equal;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_KP_Multiply )].Key = crKeyCode_KeyPad_Multiply;
		KeyStates[XKeysymToKeycode ( MainDisplay, XK_KP_Subtract )].Key = crKeyCode_KeyPad_Subtract;
		KeyCodesInitialized = true;
		}

	return KeyStates[X11KeyCode].Key;
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
	WindowAttributes.event_mask = ExposureMask | StructureNotifyMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
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

	/*XSizeHints *wind_size_hints = XAllocSizeHints();

	wind_size_hints->min_width = min_width;
	wind_size_hints->min_height = min_height;
	wind_size_hints->width = width;
	wind_size_hints->height = height;
	wind_size_hints->max_width = max_width;
	wind_size_hints->max_height = max_height;

	XSetWMNormalHints ( disp, wind, wind_size_hints );
	XFree ( wind_size_hints );*/

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
	int IsSupported;
	bool Result = XkbSetDetectableAutoRepeat ( NewWindowData->DisplayHandle, true, &IsSupported );
	if ( ( Result != true ) || ( IsSupported == false ) )
		{
		LOG_ERROR ( "Failed to set detectable auto repeat for keyboard. Key release events will not be detected when a key is held down." );
		return false;
		}
	XFlush ( NewWindowData->DisplayHandle );
	XSync ( NewWindowData->DisplayHandle, false );
	XUngrabServer ( NewWindowData->DisplayHandle );
	XStoreName ( NewWindowData->DisplayHandle, NewWindowData->X11WindowHandle, Descriptor.Title );
	XFetchName ( NewWindowData->DisplayHandle, NewWindowData->X11WindowHandle, &NewWindowData->Title );
	NewWindowData->Atoms.WM_DELETE_WINDOW = XInternAtom ( NewWindowData->DisplayHandle, "WM_DELETE_WINDOW", False );
	if ( !XSetWMProtocols ( NewWindowData->DisplayHandle, NewWindowData->X11WindowHandle, &NewWindowData->Atoms.WM_DELETE_WINDOW, 1 ) )
		{
		XDestroyWindow ( NewWindowData->DisplayHandle, NewWindowData->X11WindowHandle );
		free ( NewWindowData );
		return NULL;
		}

	PointerList_AddAtEnd ( &WindowList, ( void * ) NewWindowData );

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
		case EnterNotify:
		case LeaveNotify:
		case FocusIn:
		case FocusOut:
		case KeymapNotify:
		case Expose:
		case GraphicsExpose:
		case NoExpose:
		case VisibilityNotify:
		case CreateNotify:
		case DestroyNotify:
		case UnmapNotify:
		case MapNotify:
		case MapRequest:
		case ReparentNotify:
		case ConfigureRequest:
		case GravityNotify:
		case ResizeRequest:
		case CirculateNotify:
		case CirculateRequest:
		case PropertyNotify:
		case SelectionClear:
		case SelectionRequest:
		case SelectionNotify:
		case ColormapNotify:
		case MappingNotify:
		case GenericEvent:
			break;
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
			XKeyPressedEvent *KeyEvent = ( XKeyPressedEvent* ) &Event;
			bool State = ( KeyEvent->type == KeyPress );

			if ( ( WindowManagerCallbacks.KeyStateChanged ) && ( State != KeyStates[KeyEvent->keycode].State ) )
				{
				crKeyCode KeyCode = crX11TranslateKeyCodeTocrKeyCode ( KeyEvent->keycode );
				WindowManagerCallbacks.KeyStateChanged ( WindowData->WindowHandle, KeyCode, State );
				}
			KeyStates[KeyEvent->keycode].State = State;
			break;
			}
		case ButtonPress:
		case ButtonRelease:
			{
			XButtonPressedEvent *ButtonEvent = ( XButtonPressedEvent* ) &Event;
			if ( WindowManagerCallbacks.MouseButtonStateChanged )
				{
				bool State = ( ButtonEvent->type == ButtonPress );
				unsigned Button = ButtonEvent->button;
				WindowManagerCallbacks.MouseButtonStateChanged ( WindowData->WindowHandle, Button, State );
				}
			break;
			}
		case MotionNotify:
			{
			XMotionEvent *MotionEvent = ( XMotionEvent* ) &Event;
			WindowData->LastMouseCursorPosition.x = MotionEvent->x;
			WindowData->LastMouseCursorPosition.y = MotionEvent->y;
			LOG_DEBUG ( "Mouse moved in window '%s' to position %d, %d", WindowData->Title, WindowData->LastMouseCursorPosition.x, WindowData->LastMouseCursorPosition.y );
			if ( WindowManagerCallbacks.MouseMoved )
				{
				WindowManagerCallbacks.MouseMoved ( WindowData->WindowHandle, WindowData->LastMouseCursorPosition );
				}
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

				if ( WindowData->Title != NULL )
					{
					XFree ( WindowData->Title );
					WindowData->Title = NULL;
					}

				XDestroyWindow ( WindowData->DisplayHandle, WindowData->X11WindowHandle );
				PointerList_DestroyNode ( &WindowList, Node );
				}
			break;
			}
		}
	}

bool crX11UpdateWindows ( void )
	{
	XEvent Event = {};
	PointerListNode *Node = PointerList_GetFirst ( &WindowList );
	while ( Node != NULL )
		{
		struct InternalX11WindowData *X11WindowData = ( struct InternalX11WindowData * ) PointerList_GetNodeData ( Node );

		while ( XPending ( X11WindowData->DisplayHandle ) )
			{
			XNextEvent ( X11WindowData->DisplayHandle, &Event );
			ProcessXEvent ( Event );
			}
		Node = PointerList_GetNextNode ( Node );
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

bool crX11GetWindowPosition ( const crWindowHandle WindowHandle, ivec2 * Position )
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

bool crX11GetWindowDimensions ( const crWindowHandle WindowHandle, uvec2 * Dimensions )
	{
	struct InternalX11WindowData *WindowData = GetInternalX11WindowDataFromcrWindowHandle ( WindowHandle );
	if ( WindowData == NULL )
		return false;
	*Dimensions = WindowData->Dimensions;
	return true;
	}

bool crX11GetWindowClientAreaDimensions ( const crWindowHandle WindowHandle, uvec2 * Dimensions )
	{
	return crGetWindowDimensions ( WindowHandle, Dimensions );
	}

bool crX11SetWindowTitle ( const crWindowHandle WindowHandle, const char *Title )
	{
	struct InternalX11WindowData *WindowData = GetInternalX11WindowDataFromcrWindowHandle ( WindowHandle );
	if ( WindowData == NULL )
		return false;

	// Two different ways to set the window title.
#if 1
	XTextProperty WindowTitleProperty = {0};
	WindowTitleProperty.value = ( unsigned char * ) Title;
	WindowTitleProperty.encoding = XA_STRING;
	WindowTitleProperty.format = 8;
	WindowTitleProperty.nitems = strlen ( Title );
	XSetWMName ( WindowData->DisplayHandle, WindowData->X11WindowHandle, &WindowTitleProperty );
#else
	//XStoreName ( WindowData->DisplayHandle, WindowData->X11WindowHandle, Title );
#endif
	return true;
	}

const char *crX11GetWindowTitle ( const crWindowHandle WindowHandle )
	{
	struct InternalX11WindowData *WindowData = GetInternalX11WindowDataFromcrWindowHandle ( WindowHandle );
	if ( WindowData == NULL )
		return false;

	if ( WindowData->Title != NULL )
		{
		XFree ( WindowData->Title );
		WindowData->Title = NULL;
		}
	XFetchName ( WindowData->DisplayHandle, WindowData->X11WindowHandle, &WindowData->Title );
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

bool crX11SetMousePosition ( const crWindowHandle WindowHandle, const ivec2 Position )
	{
	struct InternalX11WindowData *WindowData = GetInternalX11WindowDataFromcrWindowHandle ( WindowHandle );
	if ( WindowData == NULL )
		return false;
	XWarpPointer ( WindowData->DisplayHandle, None, WindowData->X11WindowHandle, 0, 0, 0, 0, Position.x, Position.y );
	return true;
	}

bool crX11GetMousePosition ( const crWindowHandle WindowHandle, ivec2 * Position )
	{
	struct InternalX11WindowData *WindowData = GetInternalX11WindowDataFromcrWindowHandle ( WindowHandle );
	if ( WindowData == NULL )
		return false;

	Window ChildWindow, RootWindow;
	int ChildX, ChildY, RootX, RootY;
	unsigned MaskReturn;
	if ( XQueryPointer ( WindowData->DisplayHandle, WindowData->X11WindowHandle, &ChildWindow, &RootWindow, &RootX, &RootY, &ChildX, &ChildY, &MaskReturn ) == false )
		return false;
	if ( RootWindow != WindowData->X11WindowHandle )
		return false;
	return true;
	}
