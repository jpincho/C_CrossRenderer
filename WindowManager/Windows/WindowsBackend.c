#include "WindowsBackend.h"

#include "../Internal/WindowManagerInternal.h"
#include <Platform/Logger.h>
#include <Windows.h>
#include <windowsx.h>
#include <assert.h>

static const char *WindowClassName = "CrossWindowClass";
struct InternalWindowData
	{
	ivec2 LastMouseCursorPosition;
	};

typedef struct
	{
	crKeyCode Key;
	bool State;
	} KeyState;

static KeyState KeyStates[512] = { 0 };

static WPARAM MapLeftRightKeys ( WPARAM vk, LPARAM lParam )
	{
	WPARAM new_vk = vk;
	UINT scancode = ( lParam & 0x00ff0000 ) >> 16;
	int extended = ( lParam & 0x01000000 ) != 0;

	switch ( vk )
		{
		case VK_SHIFT:
			new_vk = MapVirtualKey ( scancode, MAPVK_VSC_TO_VK_EX );
			break;
		case VK_CONTROL:
			new_vk = extended ? VK_RCONTROL : VK_LCONTROL;
			break;
		case VK_MENU:
			new_vk = extended ? VK_RMENU : VK_LMENU;
			break;
		default:
			// not a key we map from generic to left/right specialized
			//  just return it.
			new_vk = vk;
			break;
		}

	return new_vk;
	}

static crKeyCode crWindowsTranslateKeyCodeTocrKeyCode ( const unsigned WinKeyCode )
	{
	static bool KeyCodesInitialized = false;

	if ( KeyCodesInitialized == false )
		{
		for ( unsigned KeyCode = 0; KeyCode < 512; ++KeyCode )
			{
			KeyStates[KeyCode].Key = -1;
			KeyStates[KeyCode].State = false;
			}

		for ( unsigned KeyCode = 0; KeyCode < 10; ++KeyCode )
			KeyStates['0' + KeyCode].Key = crKeyCode_0 + KeyCode;

		for ( unsigned KeyCode = 0; KeyCode < 26; ++KeyCode )
			KeyStates['A' + KeyCode].Key = crKeyCode_A + KeyCode;

		KeyStates[VK_SPACE].Key = crKeyCode_Space;
		KeyStates[VK_ESCAPE].Key = crKeyCode_Escape;
		KeyStates[VK_BACK].Key = crKeyCode_Backspace;
		KeyStates[VK_TAB].Key = crKeyCode_Tab;
		//KeyStates[VK_CLEAR].Key = 0;
		KeyStates[VK_RETURN].Key = crKeyCode_Enter;
		KeyStates[VK_LSHIFT].Key = crKeyCode_LeftShift;
		KeyStates[VK_RSHIFT].Key = crKeyCode_RightShift;
		KeyStates[VK_LCONTROL].Key = crKeyCode_LeftControl;
		KeyStates[VK_RCONTROL].Key = crKeyCode_RightControl;
		KeyStates[VK_MENU].Key = crKeyCode_Menu;
		KeyStates[VK_PAUSE].Key = crKeyCode_Pause;
		KeyStates[VK_CAPITAL].Key = crKeyCode_CapsLock;
		/*KeyStates[VK_KANA].Key = 0;
		KeyStates[VK_HANGEUL].Key = 0;
		KeyStates[VK_HANGUL].Key = 0;
		KeyStates[VK_IME_ON].Key = 0;
		KeyStates[VK_JUNJA].Key = 0;
		KeyStates[VK_FINAL].Key = 0;
		KeyStates[VK_HANJA].Key = 0;
		KeyStates[VK_KANJI].Key = 0;
		KeyStates[VK_IME_OFF].Key = 0;
		KeyStates[VK_CONVERT].Key = 0;
		KeyStates[VK_NONCONVERT].Key = 0;
		KeyStates[VK_ACCEPT].Key = 0;
		KeyStates[VK_MODECHANGE].Key = 0;*/
		KeyStates[VK_PRIOR].Key = crKeyCode_PageUp;
		KeyStates[VK_NEXT].Key = crKeyCode_PageDown;
		KeyStates[VK_END].Key = crKeyCode_End;
		KeyStates[VK_HOME].Key = crKeyCode_Home;
		KeyStates[VK_LEFT].Key = crKeyCode_Left;
		KeyStates[VK_UP].Key = crKeyCode_Up;
		KeyStates[VK_RIGHT].Key = crKeyCode_Right;
		KeyStates[VK_DOWN].Key = crKeyCode_Down;
		//KeyStates[VK_SELECT].Key = crKeyCode;
		KeyStates[VK_PRINT].Key = crKeyCode_PrintScreen;
		//KeyStates[VK_EXECUTE].Key = crKeyCode;
		//KeyStates[VK_SNAPSHOT].Key = crKeyCode;
		KeyStates[VK_INSERT].Key = crKeyCode_Insert;
		KeyStates[VK_DELETE].Key = crKeyCode_Delete;
		//KeyStates[VK_HELP].Key = crKeyCode;
		KeyStates[VK_LWIN].Key = crKeyCode_LeftSuper;
		KeyStates[VK_RWIN].Key = crKeyCode_RightSuper;
		//KeyStates[VK_APPS].Key = crKeyCode;
		//KeyStates[VK_SLEEP].Key = crKeyCode;
		for ( unsigned KeyCode = 0; KeyCode < 10; ++KeyCode )
			KeyStates[VK_NUMPAD0 + KeyCode].Key = crKeyCode_KeyPad_0 + KeyCode;

		KeyStates[VK_MULTIPLY].Key = crKeyCode_KeyPad_Multiply;
		KeyStates[VK_ADD].Key = crKeyCode_KeyPad_Add;
		//KeyStates[VK_SEPARATOR].Key = crKeyCode;
		KeyStates[VK_SUBTRACT].Key = crKeyCode_KeyPad_Subtract;
		KeyStates[VK_DECIMAL].Key = crKeyCode_KeyPad_Decimal;
		KeyStates[VK_DIVIDE].Key = crKeyCode_KeyPad_Divide;

		for ( unsigned KeyCode = 0; KeyCode < 24; ++KeyCode )
			KeyStates[VK_F1 + KeyCode].Key = crKeyCode_F1 + KeyCode;

		KeyCodesInitialized = true;
		}

	return KeyStates[WinKeyCode].Key;
	}

static LRESULT WindowProcedure ( HWND WindowHandle, UINT Message, WPARAM wParam, LPARAM lParam )
	{
	struct InternalWindowData *WindowData = ( struct InternalWindowData * ) GetWindowLongPtr ( WindowHandle, GWLP_USERDATA );
	switch ( Message )
		{
		case WM_CREATE:
			{
			assert ( WindowData == NULL );
			WindowData = calloc ( 1, sizeof ( struct InternalWindowData ) );
			SetWindowLongPtr ( WindowHandle, GWLP_USERDATA, ( LONG_PTR ) WindowData );
			break;
			}
		case WM_DESTROY:
			{
			SAFE_DEL_C ( WindowData );
			break;
			}
		case WM_CLOSE:
			{
			if ( WindowManagerCallbacks.WindowClosed )
				WindowManagerCallbacks.WindowClosed ( WindowHandle );
			break;
			}
		case WM_SIZE:
			{
			if ( WindowManagerCallbacks.WindowResized )
				{
				uvec2 NewSize;
				NewSize.x = LOWORD ( lParam );
				NewSize.y = HIWORD ( lParam );
				WindowManagerCallbacks.WindowResized ( WindowHandle, NewSize );
				}
			break;
			}
		case WM_EXITSIZEMOVE:
			{
			if ( WindowManagerCallbacks.EndWindowResized )
				{
				uvec2 NewSize;
				NewSize.x = LOWORD ( lParam );
				NewSize.y = HIWORD ( lParam );
				WindowManagerCallbacks.EndWindowResized ( WindowHandle, NewSize );
				}
			break;
			}
		case WM_MOVE:
			{
			if ( WindowManagerCallbacks.WindowMoved )
				{
				ivec2 NewPosition;
				NewPosition.x = LOWORD ( lParam );
				NewPosition.y = HIWORD ( lParam );
				WindowManagerCallbacks.WindowMoved ( WindowHandle, NewPosition );
				}
			break;
			}

		case WM_MOUSEMOVE:
			{
			assert ( WindowData != NULL );
			WindowData->LastMouseCursorPosition.x = GET_X_LPARAM ( lParam );
			WindowData->LastMouseCursorPosition.y = GET_Y_LPARAM ( lParam );
			if ( WindowManagerCallbacks.MouseMoved )
				{
				WindowManagerCallbacks.MouseMoved ( WindowHandle, WindowData->LastMouseCursorPosition );
				}
			break;
			}
		case WM_MOUSEWHEEL:
			{
			assert ( WindowData != NULL );
			WindowData->LastMouseCursorPosition.x = GET_X_LPARAM ( lParam );
			WindowData->LastMouseCursorPosition.y = GET_Y_LPARAM ( lParam );
			if ( WindowManagerCallbacks.MouseWheel )
				{
				WindowManagerCallbacks.MouseWheel ( WindowHandle, GET_WHEEL_DELTA_WPARAM ( wParam ) );
				}
			break;
			}
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			{
			assert ( WindowData != NULL );
			bool Click = false;
			int Button = 0;
			switch ( Message )
				{
				case WM_LBUTTONDOWN:
					Click = true;
					Button = 0;
					break;
				case WM_RBUTTONDOWN:
					Click = true;
					Button = 1;
					break;
				case WM_MBUTTONDOWN:
					Click = true;
					Button = 2;
					break;
				case WM_LBUTTONUP:
					Click = false;
					Button = 0;
					break;
				case WM_RBUTTONUP:
					Click = false;
					Button = 1;
					break;
				case WM_MBUTTONUP:
					Click = false;
					Button = 2;
					break;
				}
			WindowData->LastMouseCursorPosition.x = GET_X_LPARAM ( lParam );
			WindowData->LastMouseCursorPosition.y = GET_Y_LPARAM ( lParam );
			if ( WindowManagerCallbacks.MouseButtonStateChanged )
				{
				WindowManagerCallbacks.MouseButtonStateChanged ( WindowHandle, Button, Click );
				}
			break;
			}
		case WM_SETFOCUS:
			{
			if ( WindowManagerCallbacks.WindowFocusChanged )
				{
				WindowManagerCallbacks.WindowFocusChanged ( WindowHandle, true );
				}
			break;
			}
		case WM_KILLFOCUS:
			{
			if ( WindowManagerCallbacks.WindowFocusChanged )
				{
				WindowManagerCallbacks.WindowFocusChanged ( WindowHandle, false );
				}
			break;
			}
		case WM_KEYDOWN:
		case WM_KEYUP:
			{
			bool State = ( Message == WM_KEYDOWN );
			WORD ScanCode = ( HIWORD ( lParam ) & ( KF_EXTENDED | 0xff ) );
			bool WasKeyDown = ( HIWORD ( lParam ) & KF_REPEAT ) == KF_REPEAT; // previous key-state flag, 1 on autorepeat
			WORD VirtualKeyCode = MapLeftRightKeys ( wParam, lParam );

			if ( ( Message == WM_KEYDOWN ) && ( WasKeyDown == true ) )
				break;

			if ( WindowManagerCallbacks.KeyStateChanged )
				{
				crKeyCode KeyCode = crWindowsTranslateKeyCodeTocrKeyCode ( VirtualKeyCode );
				WindowManagerCallbacks.KeyStateChanged ( WindowHandle, KeyCode, State );
				}
			KeyStates[VirtualKeyCode].State = State;
			break;
			}
		}
	return DefWindowProc ( WindowHandle, Message, wParam, lParam );
	}

crWindowHandle crWindowsCreateNewWindow ( const crRenderWindowDescriptor Descriptor )
	{
	static WNDCLASSEXA CRWindowClass = { 0 };
	if ( CRWindowClass.cbSize == 0 )
		{
		CRWindowClass.cbSize = sizeof ( CRWindowClass );
		CRWindowClass.style = CS_OWNDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		CRWindowClass.lpfnWndProc = ( WNDPROC ) WindowProcedure;
		CRWindowClass.cbClsExtra = 0;
		CRWindowClass.cbWndExtra = 0;
		CRWindowClass.hInstance = GetModuleHandle ( NULL );
		CRWindowClass.hCursor = LoadCursor ( NULL, IDC_ARROW );
		CRWindowClass.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
		CRWindowClass.lpszMenuName = NULL;
		CRWindowClass.lpszClassName = WindowClassName;

		if ( RegisterClassExA ( &CRWindowClass ) == 0 )
			{
			memset ( &CRWindowClass, 0, sizeof ( CRWindowClass ) );
			return NULL;
			}
		}

	DWORD Style = 0;
	DWORD ExStyle = 0;
	ExStyle |= WS_EX_APPWINDOW;
	Style |= WS_MAXIMIZEBOX;
	Style |= WS_MINIMIZEBOX;
	Style |= WS_SYSMENU;
	Style |= WS_VISIBLE;
	Style |= WS_SIZEBOX;

	HWND NewHWND = CreateWindowExA ( ExStyle, WindowClassName, Descriptor.Title, Style,
	                                 Descriptor.Position.x, Descriptor.Position.y,
	                                 Descriptor.Size.x, Descriptor.Size.y,
	                                 NULL,
	                                 NULL,
	                                 GetModuleHandle ( NULL ),
	                                 NULL );
	if ( NewHWND == NULL )
		return NULL;
	LOG_DEBUG ( "Finished creating window %dx%d with size %ux%u. Title '%s'",
	            Descriptor.Position.x, Descriptor.Position.y,
	            Descriptor.Size.x, Descriptor.Size.y,
	            Descriptor.Title );

	PointerList_AddAtEnd ( &WindowList, NewHWND );
	return NewHWND;
	}

bool crWindowsDestroyWindow ( const crWindowHandle WindowHandle )
	{
	if ( IsWindow ( WindowHandle ) )
		{
		DestroyWindow ( WindowHandle );
		return true;
		}
	PointerListNode *node = PointerList_Find ( &WindowList, PointerList_GetFirst ( &WindowList ), WindowHandle );
	if ( node )
		PointerList_DestroyNode ( &WindowList, node );
	return false;
	}

bool crWindowsUpdateWindows ( void )
	{
	MSG Message;
	// Process all pending messages
	int Result = PeekMessage ( &Message, NULL, 0, 0, PM_REMOVE );
	while ( Result )
		{
		TranslateMessage ( &Message );
		DispatchMessage ( &Message );
		Result = PeekMessage ( &Message, NULL, 0, 0, PM_REMOVE );
		}

	return true;
	}

bool crWindowsSetWindowPosition ( const crWindowHandle WindowHandle, const ivec2 Position )
	{
	if ( IsWindow ( WindowHandle ) )
		{
		SetWindowPos ( WindowHandle, NULL, Position.x, Position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
		return true;
		}
	return false;
	}

bool crWindowsGetWindowPosition ( const crWindowHandle WindowHandle, ivec2 *Position )
	{
	if ( IsWindow ( WindowHandle ) )
		{
		RECT Rect;
		GetWindowRect ( WindowHandle, &Rect );
		Position->x = Rect.left;
		Position->y = Rect.top;
		return true;
		}
	return false;
	}

bool crWindowsSetWindowDimensions ( const crWindowHandle WindowHandle, const uvec2 Dimensions )
	{
	if ( IsWindow ( WindowHandle ) )
		{
		SetWindowPos ( WindowHandle, NULL, 0, 0, Dimensions.x, Dimensions.y, SWP_NOMOVE | SWP_NOZORDER );
		return true;
		}
	return false;
	}

bool crWindowsGetWindowDimensions ( const crWindowHandle WindowHandle, uvec2 *Dimensions )
	{
	if ( IsWindow ( WindowHandle ) )
		{
		RECT Rect;
		GetWindowRect ( WindowHandle, &Rect );
		Dimensions->x = Rect.right - Rect.left;
		Dimensions->y = Rect.bottom - Rect.top;
		return true;
		}
	return false;
	}

bool crWindowsGetWindowClientAreaDimensions ( const crWindowHandle WindowHandle, uvec2 *Dimensions )
	{
	if ( IsWindow ( WindowHandle ) )
		{
		RECT Rect;
		GetClientRect ( WindowHandle, &Rect );
		Dimensions->x = Rect.right - Rect.left;
		Dimensions->y = Rect.bottom - Rect.top;
		return true;
		}
	return false;
	}

bool crWindowsSetWindowTitle ( const crWindowHandle WindowHandle, const char *Title )
	{
	if ( IsWindow ( WindowHandle ) )
		{
		SetWindowTextA ( WindowHandle, Title );
		return true;
		}
	return false;
	}

const char *crWindowsGetWindowTitle ( const crWindowHandle WindowHandle )
	{
	static char Title[128];
	if ( IsWindow ( WindowHandle ) )
		{
		GetWindowTextA ( WindowHandle, Title, sizeof ( Title ) );
		return Title;
		}
	return NULL;
	}

bool crWindowsActivateWindow ( const crWindowHandle WindowHandle )
	{
	UNUSED ( WindowHandle );
	//HDC WindowDC = GetWindowDC(WindowHandle);
	return true;
	}

bool crWindowsSetMousePosition ( const crWindowHandle WindowHandle, const ivec2 Position )
	{
	struct InternalWindowData *WindowData = ( struct InternalWindowData * ) GetWindowLongPtr ( WindowHandle, GWLP_USERDATA );
	if ( WindowData == NULL )
		return false;
	SetCursorPos ( Position.x, Position.y );
	WindowData->LastMouseCursorPosition = Position;
	return true;
	}

bool crWindowsGetMousePosition ( const crWindowHandle WindowHandle, ivec2 *Position )
	{
	struct InternalWindowData *WindowData = ( struct InternalWindowData * ) GetWindowLongPtr ( WindowHandle, GWLP_USERDATA );
	if ( WindowData == NULL )
		return false;
	*Position = WindowData->LastMouseCursorPosition;
	return true;
	}
