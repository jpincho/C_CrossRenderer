#include "../Internal/WindowManagerInternal.h"
#include <Platform/Logger.h>
#include <Windows.h>
#include <windowsx.h>
#include <assert.h>

static const TCHAR *WindowClassName = TEXT("GUILibWindowClass");
struct InternalWindowData
	{
	ivec2 LastMouseCursorPosition;
	};

static short KeyCodes[512] = { 0 }, ScanCodes[512];


static void InitializeKeyTranslationTables(void)
	{
	short scancode;

	memset(KeyCodes, -1, sizeof(KeyCodes));
	memset(ScanCodes, -1, sizeof(ScanCodes));

	KeyCodes[0x00B] = CR_KEY_0;
	KeyCodes[0x002] = CR_KEY_1;
	KeyCodes[0x003] = CR_KEY_2;
	KeyCodes[0x004] = CR_KEY_3;
	KeyCodes[0x005] = CR_KEY_4;
	KeyCodes[0x006] = CR_KEY_5;
	KeyCodes[0x007] = CR_KEY_6;
	KeyCodes[0x008] = CR_KEY_7;
	KeyCodes[0x009] = CR_KEY_8;
	KeyCodes[0x00A] = CR_KEY_9;
	KeyCodes[0x01E] = CR_KEY_A;
	KeyCodes[0x030] = CR_KEY_B;
	KeyCodes[0x02E] = CR_KEY_C;
	KeyCodes[0x020] = CR_KEY_D;
	KeyCodes[0x012] = CR_KEY_E;
	KeyCodes[0x021] = CR_KEY_F;
	KeyCodes[0x022] = CR_KEY_G;
	KeyCodes[0x023] = CR_KEY_H;
	KeyCodes[0x017] = CR_KEY_I;
	KeyCodes[0x024] = CR_KEY_J;
	KeyCodes[0x025] = CR_KEY_K;
	KeyCodes[0x026] = CR_KEY_L;
	KeyCodes[0x032] = CR_KEY_M;
	KeyCodes[0x031] = CR_KEY_N;
	KeyCodes[0x018] = CR_KEY_O;
	KeyCodes[0x019] = CR_KEY_P;
	KeyCodes[0x010] = CR_KEY_Q;
	KeyCodes[0x013] = CR_KEY_R;
	KeyCodes[0x01F] = CR_KEY_S;
	KeyCodes[0x014] = CR_KEY_T;
	KeyCodes[0x016] = CR_KEY_U;
	KeyCodes[0x02F] = CR_KEY_V;
	KeyCodes[0x011] = CR_KEY_W;
	KeyCodes[0x02D] = CR_KEY_X;
	KeyCodes[0x015] = CR_KEY_Y;
	KeyCodes[0x02C] = CR_KEY_Z;

	KeyCodes[0x028] = CR_KEY_APOSTROPHE;
	KeyCodes[0x02B] = CR_KEY_BACKSLASH;
	KeyCodes[0x033] = CR_KEY_COMMA;
	KeyCodes[0x00D] = CR_KEY_EQUAL;
	KeyCodes[0x029] = CR_KEY_GRAVE_ACCENT;
	KeyCodes[0x01A] = CR_KEY_LEFT_BRACKET;
	KeyCodes[0x00C] = CR_KEY_MINUS;
	KeyCodes[0x034] = CR_KEY_PERIOD;
	KeyCodes[0x01B] = CR_KEY_RIGHT_BRACKET;
	KeyCodes[0x027] = CR_KEY_SEMICOLON;
	KeyCodes[0x035] = CR_KEY_SLASH;
	KeyCodes[0x056] = CR_KEY_WORLD_2;

	KeyCodes[0x00E] = CR_KEY_BACKSPACE;
	KeyCodes[0x153] = CR_KEY_DELETE;
	KeyCodes[0x14F] = CR_KEY_END;
	KeyCodes[0x01C] = CR_KEY_ENTER;
	KeyCodes[0x001] = CR_KEY_ESCAPE;
	KeyCodes[0x147] = CR_KEY_HOME;
	KeyCodes[0x152] = CR_KEY_INSERT;
	KeyCodes[0x15D] = CR_KEY_MENU;
	KeyCodes[0x151] = CR_KEY_PAGE_DOWN;
	KeyCodes[0x149] = CR_KEY_PAGE_UP;
	KeyCodes[0x045] = CR_KEY_PAUSE;
	KeyCodes[0x039] = CR_KEY_SPACE;
	KeyCodes[0x00F] = CR_KEY_TAB;
	KeyCodes[0x03A] = CR_KEY_CAPS_LOCK;
	KeyCodes[0x145] = CR_KEY_NUM_LOCK;
	KeyCodes[0x046] = CR_KEY_SCROLL_LOCK;
	KeyCodes[0x03B] = CR_KEY_F1;
	KeyCodes[0x03C] = CR_KEY_F2;
	KeyCodes[0x03D] = CR_KEY_F3;
	KeyCodes[0x03E] = CR_KEY_F4;
	KeyCodes[0x03F] = CR_KEY_F5;
	KeyCodes[0x040] = CR_KEY_F6;
	KeyCodes[0x041] = CR_KEY_F7;
	KeyCodes[0x042] = CR_KEY_F8;
	KeyCodes[0x043] = CR_KEY_F9;
	KeyCodes[0x044] = CR_KEY_F10;
	KeyCodes[0x057] = CR_KEY_F11;
	KeyCodes[0x058] = CR_KEY_F12;
	KeyCodes[0x064] = CR_KEY_F13;
	KeyCodes[0x065] = CR_KEY_F14;
	KeyCodes[0x066] = CR_KEY_F15;
	KeyCodes[0x067] = CR_KEY_F16;
	KeyCodes[0x068] = CR_KEY_F17;
	KeyCodes[0x069] = CR_KEY_F18;
	KeyCodes[0x06A] = CR_KEY_F19;
	KeyCodes[0x06B] = CR_KEY_F20;
	KeyCodes[0x06C] = CR_KEY_F21;
	KeyCodes[0x06D] = CR_KEY_F22;
	KeyCodes[0x06E] = CR_KEY_F23;
	KeyCodes[0x076] = CR_KEY_F24;
	KeyCodes[0x038] = CR_KEY_LEFT_ALT;
	KeyCodes[0x01D] = CR_KEY_LEFT_CONTROL;
	KeyCodes[0x02A] = CR_KEY_LEFT_SHIFT;
	KeyCodes[0x15B] = CR_KEY_LEFT_SUPER;
	KeyCodes[0x137] = CR_KEY_PRINT_SCREEN;
	KeyCodes[0x138] = CR_KEY_RIGHT_ALT;
	KeyCodes[0x11D] = CR_KEY_RIGHT_CONTROL;
	KeyCodes[0x036] = CR_KEY_RIGHT_SHIFT;
	KeyCodes[0x15C] = CR_KEY_RIGHT_SUPER;
	KeyCodes[0x150] = CR_KEY_DOWN;
	KeyCodes[0x14B] = CR_KEY_LEFT;
	KeyCodes[0x14D] = CR_KEY_RIGHT;
	KeyCodes[0x148] = CR_KEY_UP;

	KeyCodes[0x052] = CR_KEY_KP_0;
	KeyCodes[0x04F] = CR_KEY_KP_1;
	KeyCodes[0x050] = CR_KEY_KP_2;
	KeyCodes[0x051] = CR_KEY_KP_3;
	KeyCodes[0x04B] = CR_KEY_KP_4;
	KeyCodes[0x04C] = CR_KEY_KP_5;
	KeyCodes[0x04D] = CR_KEY_KP_6;
	KeyCodes[0x047] = CR_KEY_KP_7;
	KeyCodes[0x048] = CR_KEY_KP_8;
	KeyCodes[0x049] = CR_KEY_KP_9;
	KeyCodes[0x04E] = CR_KEY_KP_ADD;
	KeyCodes[0x053] = CR_KEY_KP_DECIMAL;
	KeyCodes[0x135] = CR_KEY_KP_DIVIDE;
	KeyCodes[0x11C] = CR_KEY_KP_ENTER;
	KeyCodes[0x059] = CR_KEY_KP_EQUAL;
	KeyCodes[0x037] = CR_KEY_KP_MULTIPLY;
	KeyCodes[0x04A] = CR_KEY_KP_SUBTRACT;

	for (scancode = 0; scancode < 512; scancode++)
		{
		if (KeyCodes[scancode] > 0)
			ScanCodes[KeyCodes[scancode]] = scancode;
		}
	}

LRESULT WindowProcedure(HWND WindowHandle, UINT Message, WPARAM wParam, LPARAM lParam)
	{
	struct InternalWindowData *WindowData = (struct InternalWindowData *)GetWindowLongPtr(WindowHandle, GWLP_USERDATA);
	switch (Message)
		{
		case WM_CREATE:
			{
			assert(WindowData == NULL);
			WindowData = calloc(1, sizeof(struct InternalWindowData));
			SetWindowLongPtr(WindowHandle, GWLP_USERDATA, (LONG_PTR)WindowData);
			break;
			}
		case WM_DESTROY:
			{
			SAFE_DEL_C(WindowData);
			break;
			}
		case WM_CLOSE:
			{
			if (WindowManagerCallbacks.WindowClosed)
				WindowManagerCallbacks.WindowClosed(WindowHandle);
			break;
			}
		case WM_SIZE:
			{
			if (WindowManagerCallbacks.WindowResized)
				{
				uvec2 NewSize;
				NewSize.x = LOWORD(lParam);
				NewSize.y = HIWORD(lParam);
				WindowManagerCallbacks.WindowResized(WindowHandle, NewSize);
				}
			break;
			}
		case WM_EXITSIZEMOVE:
			{
			if (WindowManagerCallbacks.EndWindowResized)
				{
				uvec2 NewSize;
				NewSize.x = LOWORD(lParam);
				NewSize.y = HIWORD(lParam);
				WindowManagerCallbacks.EndWindowResized(WindowHandle, NewSize);
				}
			break;
			}
		case WM_MOVE:
			{
			if (WindowManagerCallbacks.WindowMoved)
				{
				ivec2 NewPosition;
				NewPosition.x = LOWORD(lParam);
				NewPosition.y = HIWORD(lParam);
				WindowManagerCallbacks.WindowMoved(WindowHandle, NewPosition);
				}
			break;
			}

		case WM_MOUSEMOVE:
			{
			assert(WindowData != NULL);
			ivec2 Delta;
			Delta.x = GET_X_LPARAM(lParam);
			Delta.y = GET_Y_LPARAM(lParam);
			Delta.x -= WindowData->LastMouseCursorPosition.x;
			Delta.y -= WindowData->LastMouseCursorPosition.y;
			WindowData->LastMouseCursorPosition.x = GET_X_LPARAM(lParam);
			WindowData->LastMouseCursorPosition.y = GET_Y_LPARAM(lParam);
			if ((WindowManagerCallbacks.MouseMoved) &&
			        ((Delta.x != 0) || (Delta.y != 0)))
				{
				WindowManagerCallbacks.MouseMoved(WindowHandle, Delta, WindowData->LastMouseCursorPosition);
				}
			break;
			}
		case WM_MOUSEWHEEL:
			{
			assert(WindowData != NULL);
			WindowData->LastMouseCursorPosition.x = GET_X_LPARAM(lParam);
			WindowData->LastMouseCursorPosition.y = GET_Y_LPARAM(lParam);
			if (WindowManagerCallbacks.MouseWheel)
				{
				WindowManagerCallbacks.MouseWheel(WindowHandle, GET_WHEEL_DELTA_WPARAM(wParam));
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
			assert(WindowData != NULL);
			bool Click = false;
			int Button = 0;
			switch (Message)
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
			WindowData->LastMouseCursorPosition.x = GET_X_LPARAM(lParam);
			WindowData->LastMouseCursorPosition.y = GET_Y_LPARAM(lParam);
			if (WindowManagerCallbacks.MouseButton)
				{
				WindowManagerCallbacks.MouseButton(WindowHandle, Button, Click);
				}
			break;
			}
		case WM_SETFOCUS:
			{
			if (WindowManagerCallbacks.WindowFocusChanged)
				{
				WindowManagerCallbacks.WindowFocusChanged(WindowHandle, true);
				}
			break;
			}
		case WM_KILLFOCUS:
			{
			if (WindowManagerCallbacks.WindowFocusChanged)
				{
				WindowManagerCallbacks.WindowFocusChanged(WindowHandle, false);
				}
			break;
			}
		}
	return DefWindowProc(WindowHandle, Message, wParam, lParam);
	}
		case WM_KEYDOWN:
		case WM_KEYUP:
			{
			if (WindowManagerCallbacks.KeyStateChanged)
				{
				bool State = (Message == WM_KEYDOWN);

				WORD ScanCode = (HIWORD(lParam) & (KF_EXTENDED | 0xff));
				bool WasKeyDown = (HIWORD(lParam) & KF_REPEAT) == KF_REPEAT;        // previous key-state flag, 1 on autorepeat

				if ((Message == WM_KEYDOWN) && (WasKeyDown == true))
					break;

				unsigned KeyCode = KeyCodes[ScanCode];
				WindowManagerCallbacks.KeyStateChanged(WindowHandle, KeyCode, State);
				}
			break;
			}
		}
	return DefWindowProc(WindowHandle, Message, wParam, lParam);
	}

bool crInitializeWindowBackend(void)
	{
	InitializeKeyTranslationTables();
	return true;
	}

crWindowHandle crCreateNewWindow(const crRenderWindowDescriptor Descriptor)
	{
	static WNDCLASSEX CRWindowClass = { 0 };
	if (CRWindowClass.cbSize == 0)
		{
		CRWindowClass.cbSize = sizeof(CRWindowClass);
		CRWindowClass.style = CS_OWNDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		CRWindowClass.lpfnWndProc = (WNDPROC)WindowProcedure;
		CRWindowClass.cbClsExtra = 0;
		CRWindowClass.cbWndExtra = 0;
		CRWindowClass.hInstance = GetModuleHandle(NULL);
		CRWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		CRWindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		CRWindowClass.lpszMenuName = NULL;
		CRWindowClass.lpszClassName = WindowClassName;

		if (RegisterClassEx(&CRWindowClass) == 0)
			{
			memset(&CRWindowClass, 0, sizeof(CRWindowClass));
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

	HWND NewHWND = CreateWindowEx(ExStyle, WindowClassName, Descriptor.Title, Style,
	                              Descriptor.Position.x, Descriptor.Position.y,
	                              Descriptor.Size.x, Descriptor.Size.y,
	                              NULL,
	                              NULL,
	                              GetModuleHandle(NULL),
	                              NULL);
	if (NewHWND == NULL)
		return NULL;
	LOG_DEBUG("Finished creating window %dx%d with size %ux%u. Title '%s'",
	          Descriptor.Position.x, Descriptor.Position.y,
	          Descriptor.Size.x, Descriptor.Size.y,
	          Descriptor.Title);

	PointerList_AddAtEnd(&WindowList, NewHWND);
	return NewHWND;
	}

bool crDestroyWindow(const crWindowHandle WindowHandle)
	{
	if (IsWindow(WindowHandle))
		{
		DestroyWindow(WindowHandle);
		return true;
		}
	pointer_list_node *node = PointerList_Find(&WindowList, PointerList_GetFirst(&WindowList), WindowHandle);
	if (node)
		PointerList_DestroyNode(&WindowList, node);
	return false;
	}

bool crUpdateWindows(const bool Wait)
	{
	MSG Message;
	// Process all pending messages
	int Result = PeekMessage(&Message, NULL, 0, 0, PM_REMOVE);
	while (Result)
		{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
		Result = PeekMessage(&Message, NULL, 0, 0, PM_REMOVE);
		}

	if (Wait)
		{
		Result = GetMessage(&Message, NULL, 0, 0);
		if (Result <= 0)
			return false;
		TranslateMessage(&Message);
		DispatchMessage(&Message);
		}
	return true;
	}

bool crSetWindowPosition(const crWindowHandle WindowHandle, const ivec2 Position)
	{
	if (IsWindow(WindowHandle))
		{
		SetWindowPos(WindowHandle, NULL, Position.x, Position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		return true;
		}
	return false;
	}

bool crGetWindowPosition(const crWindowHandle WindowHandle, ivec2 *Position)
	{
	if (IsWindow(WindowHandle))
		{
		RECT Rect;
		GetWindowRect(WindowHandle, &Rect);
		Position->x = Rect.left;
		Position->y = Rect.top;
		return true;
		}
	return false;
	}

bool crSetWindowDimensions(const crWindowHandle WindowHandle, const uvec2 Dimensions)
	{
	if (IsWindow(WindowHandle))
		{
		SetWindowPos(WindowHandle, NULL, 0, 0, Dimensions.x, Dimensions.y, SWP_NOMOVE | SWP_NOZORDER);
		return true;
		}
	return false;
	}

bool crGetWindowDimensions(const crWindowHandle WindowHandle, uvec2 *Dimensions)
	{
	if (IsWindow(WindowHandle))
		{
		RECT Rect;
		GetWindowRect(WindowHandle, &Rect);
		Dimensions->x = Rect.right - Rect.left;
		Dimensions->y = Rect.bottom - Rect.top;
		return true;
		}
	return false;
	}

bool crGetWindowClientAreaDimensions(const crWindowHandle WindowHandle, uvec2 *Dimensions)
	{
	if (IsWindow(WindowHandle))
		{
		RECT Rect;
		GetClientRect(WindowHandle, &Rect);
		Dimensions->x = Rect.right - Rect.left;
		Dimensions->y = Rect.bottom - Rect.top;
		return true;
		}
	return false;
	}

bool crSetWindowTitle(const crWindowHandle WindowHandle, const char *Title)
	{
	if (IsWindow(WindowHandle))
		{
		SetWindowTextA(WindowHandle, Title);
		return true;
		}
	return false;
	}

const char *crGetWindowTitle(const crWindowHandle WindowHandle)
	{
	static char Title[128];
	if (IsWindow(WindowHandle))
		{
		GetWindowTextA(WindowHandle, Title, sizeof(Title));
		return Title;
		}
	return NULL;
	}

bool crActivateWindow(const crWindowHandle WindowHandle)
	{
	UNUSED(WindowHandle);
	//HDC WindowDC = GetWindowDC(WindowHandle);
	return true;
	}
