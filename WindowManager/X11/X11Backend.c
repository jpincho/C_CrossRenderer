#include "X11Backend.h"
#include "X11Internal.h"
#include <X11/keysym.h>
#include "../Internal/WindowManagerInternal.h"
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

crKeyCode crX11TranslateKeyCodeTocrKeyCode ( const unsigned X11KeyCode )
	{
	static short KeyCodes[512] = { 0 };
	static bool KeyCodesInitialized = false;

	if ( KeyCodesInitialized == false )
		{
		Display *MainDisplay = XOpenDisplay ( 0 );
		memset ( KeyCodes, -1, sizeof ( KeyCodes ) );

		for ( unsigned KeyCode = 0; KeyCode < 10; ++KeyCode )
			KeyCodes[XKeysymToKeycode ( MainDisplay, KeyCode + XK_0 )] = KeyCode + crKeyCode_0;

		for ( unsigned KeyCode = 0; KeyCode < 26; ++KeyCode )
			KeyCodes[XKeysymToKeycode ( MainDisplay, KeyCode + XK_A )] = KeyCode + crKeyCode_A;

		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_apostrophe )] = crKeyCode_Apostrophe;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_backslash )] = crKeyCode_Backslash;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_comma )] = crKeyCode_Comma;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_equal )] = crKeyCode_Equal;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_grave )] = crKeyCode_GraveAccent;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_bracketleft )] = crKeyCode_LeftBracket;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_minus )] = crKeyCode_Minus;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_period )] = crKeyCode_Period;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_bracketright )] = crKeyCode_RightBracket;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_semicolon )] = crKeyCode_Semicolon;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_slash )] = crKeyCode_Slash;

		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_BackSpace )] = crKeyCode_Backspace;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Delete )] = crKeyCode_Delete;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_End )] = crKeyCode_End;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Return )] = crKeyCode_Enter;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Escape )] = crKeyCode_Escape;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Home )] = crKeyCode_Home;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Insert )] = crKeyCode_Insert;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Menu )] = crKeyCode_Menu;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Page_Down )] = crKeyCode_PageDown;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Page_Up )] = crKeyCode_PageUp;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Pause )] = crKeyCode_Pause;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_space )] = crKeyCode_Space;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Tab )] = crKeyCode_Tab;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Caps_Lock )] = crKeyCode_CapsLock;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Num_Lock )] = crKeyCode_NumLock;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Scroll_Lock )] = crKeyCode_ScrollLock;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F1 )] = crKeyCode_F1;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F2 )] = crKeyCode_F2;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F3 )] = crKeyCode_F3;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F4 )] = crKeyCode_F4;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F5 )] = crKeyCode_F5;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F6 )] = crKeyCode_F6;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F7 )] = crKeyCode_F7;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F8 )] = crKeyCode_F8;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F9 )] = crKeyCode_F9;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F10 )] = crKeyCode_F10;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F11 )] = crKeyCode_F11;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F12 )] = crKeyCode_F12;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F13 )] = crKeyCode_F13;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F14 )] = crKeyCode_F14;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F15 )] = crKeyCode_F15;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F16 )] = crKeyCode_F16;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F17 )] = crKeyCode_F17;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F18 )] = crKeyCode_F18;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F19 )] = crKeyCode_F19;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F20 )] = crKeyCode_F20;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F21 )] = crKeyCode_F21;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F22 )] = crKeyCode_F22;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F23 )] = crKeyCode_F23;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_F24 )] = crKeyCode_F24;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Alt_L )] = crKeyCode_LeftAlt;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Control_L )] = crKeyCode_LeftControl;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Shift_L )] = crKeyCode_LeftShift;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Super_L )] = crKeyCode_LeftSuper;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Print )] = crKeyCode_PrintScreen;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Alt_R )] = crKeyCode_RightAlt;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Control_R )] = crKeyCode_RightControl;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Shift_R )] = crKeyCode_RightShift;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Super_R )] = crKeyCode_RightSuper;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Down )] = crKeyCode_Down;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Left )] = crKeyCode_Left;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Right )] = crKeyCode_Right;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_Up )] = crKeyCode_Up;

		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_KP_0 )] = crKeyCode_KeyPad_0;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_KP_1 )] = crKeyCode_KeyPad_1;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_KP_2 )] = crKeyCode_KeyPad_2;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_KP_3 )] = crKeyCode_KeyPad_3;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_KP_4 )] = crKeyCode_KeyPad_4;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_KP_5 )] = crKeyCode_KeyPad_5;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_KP_6 )] = crKeyCode_KeyPad_6;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_KP_7 )] = crKeyCode_KeyPad_7;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_KP_8 )] = crKeyCode_KeyPad_8;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_KP_9 )] = crKeyCode_KeyPad_9;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_KP_Add )] = crKeyCode_KeyPad_Add;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_KP_Decimal )] = crKeyCode_KeyPad_Decimal;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_KP_Divide )] = crKeyCode_KeyPad_Divide;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_KP_Enter )] = crKeyCode_KeyPad_Enter;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_KP_Equal )] = crKeyCode_KeyPad_Equal;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_KP_Multiply )] = crKeyCode_KeyPad_Multiply;
		KeyCodes[XKeysymToKeycode ( MainDisplay, XK_KP_Subtract )] = crKeyCode_KeyPad_Subtract;
		KeyCodesInitialized = true;
		}

	return KeyCodes[X11KeyCode];
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
			XKeyPressedEvent *KeyEvent = ( XKeyPressedEvent* ) &Event;
			if ( WindowManagerCallbacks.KeyStateChanged )
				{
				bool State = ( KeyEvent->type == KeyPress );
				crKeyCode KeyCode = crX11TranslateKeyCodeTocrKeyCode ( KeyEvent->keycode );
				WindowManagerCallbacks.KeyStateChanged ( WindowData->WindowHandle, KeyCode, State );
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
