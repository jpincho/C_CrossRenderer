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

static short KeyCodes[512] = { 0 }, ScanCodes[512];

static void InitializeKeyTranslationTables ( void )
	{
	short scancode;

	memset ( KeyCodes, -1, sizeof ( KeyCodes ) );
	memset ( ScanCodes, -1, sizeof ( ScanCodes ) );

	KeyCodes[0x00B] = crKeyCode_0;
	KeyCodes[0x002] = crKeyCode_1;
	KeyCodes[0x003] = crKeyCode_2;
	KeyCodes[0x004] = crKeyCode_3;
	KeyCodes[0x005] = crKeyCode_4;
	KeyCodes[0x006] = crKeyCode_5;
	KeyCodes[0x007] = crKeyCode_6;
	KeyCodes[0x008] = crKeyCode_7;
	KeyCodes[0x009] = crKeyCode_8;
	KeyCodes[0x00A] = crKeyCode_9;
	KeyCodes[0x01E] = crKeyCode_A;
	KeyCodes[0x030] = crKeyCode_B;
	KeyCodes[0x02E] = crKeyCode_C;
	KeyCodes[0x020] = crKeyCode_D;
	KeyCodes[0x012] = crKeyCode_E;
	KeyCodes[0x021] = crKeyCode_F;
	KeyCodes[0x022] = crKeyCode_G;
	KeyCodes[0x023] = crKeyCode_H;
	KeyCodes[0x017] = crKeyCode_I;
	KeyCodes[0x024] = crKeyCode_J;
	KeyCodes[0x025] = crKeyCode_K;
	KeyCodes[0x026] = crKeyCode_L;
	KeyCodes[0x032] = crKeyCode_M;
	KeyCodes[0x031] = crKeyCode_N;
	KeyCodes[0x018] = crKeyCode_O;
	KeyCodes[0x019] = crKeyCode_P;
	KeyCodes[0x010] = crKeyCode_Q;
	KeyCodes[0x013] = crKeyCode_R;
	KeyCodes[0x01F] = crKeyCode_S;
	KeyCodes[0x014] = crKeyCode_T;
	KeyCodes[0x016] = crKeyCode_U;
	KeyCodes[0x02F] = crKeyCode_V;
	KeyCodes[0x011] = crKeyCode_W;
	KeyCodes[0x02D] = crKeyCode_X;
	KeyCodes[0x015] = crKeyCode_Y;
	KeyCodes[0x02C] = crKeyCode_Z;

	KeyCodes[0x028] = crKeyCode_Apostrophe;
	KeyCodes[0x02B] = crKeyCode_Backslash;
	KeyCodes[0x033] = crKeyCode_Comma;
	KeyCodes[0x00D] = crKeyCode_Equal;
	KeyCodes[0x029] = crKeyCode_GraveAccent;
	KeyCodes[0x01A] = crKeyCode_LeftBracket;
	KeyCodes[0x00C] = crKeyCode_Minus;
	KeyCodes[0x034] = crKeyCode_Period;
	KeyCodes[0x01B] = crKeyCode_RightBracket;
	KeyCodes[0x027] = crKeyCode_Semicolon;
	KeyCodes[0x035] = crKeyCode_Slash;
	KeyCodes[0x056] = crKeyCode_World2;

	KeyCodes[0x00E] = crKeyCode_Backspace;
	KeyCodes[0x153] = crKeyCode_Delete;
	KeyCodes[0x14F] = crKeyCode_End;
	KeyCodes[0x01C] = crKeyCode_Enter;
	KeyCodes[0x001] = crKeyCode_Escape;
	KeyCodes[0x147] = crKeyCode_Home;
	KeyCodes[0x152] = crKeyCode_Insert;
	KeyCodes[0x15D] = crKeyCode_Menu;
	KeyCodes[0x151] = crKeyCode_PageDown;
	KeyCodes[0x149] = crKeyCode_PageUp;
	KeyCodes[0x045] = crKeyCode_Pause;
	KeyCodes[0x039] = crKeyCode_Space;
	KeyCodes[0x00F] = crKeyCode_Tab;
	KeyCodes[0x03A] = crKeyCode_CapsLock;
	KeyCodes[0x145] = crKeyCode_NumLock;
	KeyCodes[0x046] = crKeyCode_ScrollLock;
	KeyCodes[0x03B] = crKeyCode_F1;
	KeyCodes[0x03C] = crKeyCode_F2;
	KeyCodes[0x03D] = crKeyCode_F3;
	KeyCodes[0x03E] = crKeyCode_F4;
	KeyCodes[0x03F] = crKeyCode_F5;
	KeyCodes[0x040] = crKeyCode_F6;
	KeyCodes[0x041] = crKeyCode_F7;
	KeyCodes[0x042] = crKeyCode_F8;
	KeyCodes[0x043] = crKeyCode_F9;
	KeyCodes[0x044] = crKeyCode_F10;
	KeyCodes[0x057] = crKeyCode_F11;
	KeyCodes[0x058] = crKeyCode_F12;
	KeyCodes[0x064] = crKeyCode_F13;
	KeyCodes[0x065] = crKeyCode_F14;
	KeyCodes[0x066] = crKeyCode_F15;
	KeyCodes[0x067] = crKeyCode_F16;
	KeyCodes[0x068] = crKeyCode_F17;
	KeyCodes[0x069] = crKeyCode_F18;
	KeyCodes[0x06A] = crKeyCode_F19;
	KeyCodes[0x06B] = crKeyCode_F20;
	KeyCodes[0x06C] = crKeyCode_F21;
	KeyCodes[0x06D] = crKeyCode_F22;
	KeyCodes[0x06E] = crKeyCode_F23;
	KeyCodes[0x076] = crKeyCode_F24;
	KeyCodes[0x038] = crKeyCode_LeftAlt;
	KeyCodes[0x01D] = crKeyCode_LeftControl;
	KeyCodes[0x02A] = crKeyCode_LeftShift;
	KeyCodes[0x15B] = crKeyCode_LeftSuper;
	KeyCodes[0x137] = crKeyCode_PrintScreen;
	KeyCodes[0x138] = crKeyCode_RightAlt;
	KeyCodes[0x11D] = crKeyCode_RightControl;
	KeyCodes[0x036] = crKeyCode_RightShift;
	KeyCodes[0x15C] = crKeyCode_RightSuper;
	KeyCodes[0x150] = crKeyCode_Down;
	KeyCodes[0x14B] = crKeyCode_Left;
	KeyCodes[0x14D] = crKeyCode_Right;
	KeyCodes[0x148] = crKeyCode_Up;

	KeyCodes[0x052] = crKeyCode_KeyPad_0;
	KeyCodes[0x04F] = crKeyCode_KeyPad_1;
	KeyCodes[0x050] = crKeyCode_KeyPad_2;
	KeyCodes[0x051] = crKeyCode_KeyPad_3;
	KeyCodes[0x04B] = crKeyCode_KeyPad_4;
	KeyCodes[0x04C] = crKeyCode_KeyPad_5;
	KeyCodes[0x04D] = crKeyCode_KeyPad_6;
	KeyCodes[0x047] = crKeyCode_KeyPad_7;
	KeyCodes[0x048] = crKeyCode_KeyPad_8;
	KeyCodes[0x049] = crKeyCode_KeyPad_9;
	KeyCodes[0x04E] = crKeyCode_KeyPad_Add;
	KeyCodes[0x053] = crKeyCode_KeyPad_Decimal;
	KeyCodes[0x135] = crKeyCode_KeyPad_Divide;
	KeyCodes[0x11C] = crKeyCode_KeyPad_Enter;
	KeyCodes[0x059] = crKeyCode_KeyPad_Equal;
	KeyCodes[0x037] = crKeyCode_KeyPad_Multiply;
	KeyCodes[0x04A] = crKeyCode_KeyPad_Subtract;

	for ( scancode = 0; scancode < 512; scancode++ )
		{
		if ( KeyCodes[scancode] > 0 )
			ScanCodes[KeyCodes[scancode]] = scancode;
		}
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
			ivec2 Delta;
			Delta.x = GET_X_LPARAM ( lParam );
			Delta.y = GET_Y_LPARAM ( lParam );
			Delta.x -= WindowData->LastMouseCursorPosition.x;
			Delta.y -= WindowData->LastMouseCursorPosition.y;
			WindowData->LastMouseCursorPosition.x = GET_X_LPARAM ( lParam );
			WindowData->LastMouseCursorPosition.y = GET_Y_LPARAM ( lParam );
			if ( ( WindowManagerCallbacks.MouseMoved ) &&
			        ( ( Delta.x != 0 ) || ( Delta.y != 0 ) ) )
				{
				WindowManagerCallbacks.MouseMoved ( WindowHandle, Delta, WindowData->LastMouseCursorPosition );
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
			if ( WindowManagerCallbacks.MouseButton )
				{
				WindowManagerCallbacks.MouseButton ( WindowHandle, Button, Click );
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
			if ( WindowManagerCallbacks.KeyStateChanged )
				{
				bool State = ( Message == WM_KEYDOWN );

				WORD ScanCode = ( HIWORD ( lParam ) & ( KF_EXTENDED | 0xff ) );
				bool WasKeyDown = ( HIWORD ( lParam ) & KF_REPEAT ) == KF_REPEAT;   // previous key-state flag, 1 on autorepeat

				if ( ( Message == WM_KEYDOWN ) && ( WasKeyDown == true ) )
					break;

				unsigned KeyCode = KeyCodes[ScanCode];
				WindowManagerCallbacks.KeyStateChanged ( WindowHandle, KeyCode, State );
				}
			break;
			}
		}
	return DefWindowProc ( WindowHandle, Message, wParam, lParam );
	}

bool crWindowsInitializeWindowBackend ( void )
	{
	InitializeKeyTranslationTables();
	return true;
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

bool crWindowsUpdateWindows ( const bool Wait )
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

	if ( Wait )
		{
		Result = GetMessage ( &Message, NULL, 0, 0 );
		if ( Result <= 0 )
			return false;
		TranslateMessage ( &Message );
		DispatchMessage ( &Message );
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
