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
				ivec2 NewSize;
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
				ivec2 NewSize;
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

crWindowHandle crCreateNewWindow(const struct crRenderWindowDescriptor Descriptor)
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

	pointer_list_add_at_end(&WindowList, NewHWND);
	return NewHWND;
	}

bool crDestroyWindow(const crWindowHandle WindowHandle)
	{
	if (IsWindow(WindowHandle))
		{
		DestroyWindow(WindowHandle);
		return true;
		}
	pointer_list_node *node = pointer_list_find(&WindowList, pointer_list_first(&WindowList), WindowHandle);
	if (node)
		pointer_list_destroy_node(&WindowList, node);
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
