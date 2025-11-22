#include "WGLContext.h"
#include <Windows.h>
#include <glad/glad_wgl.h>

static HGLRC GLContext = NULL;

crOpenGLContext crWGLCreateContext(const crWindowHandle WindowHandle, const crRendererConfiguration NewConfiguration)
	{
	// Register the window class
	WNDCLASSEX WindowClass;
	HDC FakeDC = NULL, WindowDC = NULL;
	HWND FakeWND = NULL;
	HGLRC FakeContext = NULL;

	if (!GetClassInfoEx(GetModuleHandle(NULL), "CrossRendererFakeWindowClass", &WindowClass))
		{
		memset(&WindowClass, 0, sizeof(WNDCLASSEX));
		WindowClass.cbSize = sizeof(WNDCLASSEX);
		WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		WindowClass.lpfnWndProc = DefWindowProc;
		WindowClass.cbClsExtra = 0;
		WindowClass.cbWndExtra = 0;
		WindowClass.hInstance = GetModuleHandle(NULL);
		WindowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		WindowClass.lpszMenuName = NULL;
		WindowClass.lpszClassName = "CrossRendererFakeWindowClass";
		WindowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

		if (!RegisterClassEx(&WindowClass))
			return NULL;
		}

	// Create an OpenGL 1.1 context, so that I can get the wgl extensions, and THEN create a decent context. Fucking windows
	FakeWND = CreateWindow(
	              "CrossRendererFakeWindowClass", "Fake Window",      // window class, title
	              WS_CLIPSIBLINGS | WS_CLIPCHILDREN, // style
	              0, 0,                       // position x, y
	              1, 1,                       // width, height
	              NULL, NULL,                 // parent window, menu
	              NULL, NULL);          // instance, param
	if (FakeWND == NULL)
		return NULL;

	FakeDC = GetDC(FakeWND);

	PIXELFORMATDESCRIPTOR FakePFD;
	ZeroMemory(&FakePFD, sizeof(FakePFD));
	FakePFD.nSize = sizeof(FakePFD);
	FakePFD.nVersion = 1;
	FakePFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	FakePFD.iPixelType = PFD_TYPE_RGBA;
	FakePFD.cColorBits = 32;
	FakePFD.cAlphaBits = 8;
	FakePFD.cDepthBits = 24;

	int FakePFDID = ChoosePixelFormat(FakeDC, &FakePFD);
	if (FakePFDID == 0)
		return NULL;

	if (SetPixelFormat(FakeDC, FakePFDID, &FakePFD) == false)
		return NULL;

	FakeContext = wglCreateContext(FakeDC);

	if (FakeContext == NULL)
		goto OnError;
	if (wglMakeCurrent(FakeDC, FakeContext) == false)
		goto OnError;

	if (!gladLoadWGL(FakeDC))
		goto OnError;

	// Now that the fake context is created and I have the WGL extensions, create a decent context
	WindowDC = GetWindowDC(WindowHandle);
	const int PixelFormatAttributes[] =
		{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB, NewConfiguration.RedBits + NewConfiguration.GreenBits + NewConfiguration.BlueBits,
		WGL_ALPHA_BITS_ARB, NewConfiguration.AlphaBits,
		WGL_DEPTH_BITS_ARB, NewConfiguration.DepthBits,
		WGL_STENCIL_BITS_ARB, NewConfiguration.StencilBits,
		WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
		WGL_SAMPLES_ARB, 4,
		0
		};

	int PixelFormatID;
	UINT PixelFormatCount;
	bool PixelFormatFound = wglChoosePixelFormatARB(WindowDC, PixelFormatAttributes, NULL, 1, &PixelFormatID, &PixelFormatCount);

	if (PixelFormatFound == false || PixelFormatCount == 0)
		goto OnError;
	PIXELFORMATDESCRIPTOR PFD;
	DescribePixelFormat(WindowDC, PixelFormatID, sizeof(PFD), &PFD);
	SetPixelFormat(WindowDC, PixelFormatID, &PFD);

	int ContextAttributes[] =
		{
		//WGL_CONTEXT_MAJOR_VERSION_ARB, 4,   // Set the MAJOR version of OpenGL to 4
		//WGL_CONTEXT_MINOR_VERSION_ARB, 0,   // Set the MINOR version of OpenGL to 0
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
		0
		};

	GLContext = wglCreateContextAttribsARB(WindowDC, 0, ContextAttributes);
	if (GLContext == NULL)
		goto OnError;

	// Activate the new context
	if (!wglMakeCurrent(WindowDC, GLContext))
		goto OnError;

	// Delete the old fake context, and window
	wglDeleteContext(FakeContext);
	ReleaseDC(FakeWND, FakeDC);
	DestroyWindow(FakeWND);

	if (!gladLoadWGL(WindowDC))
		goto OnError;
	ShowWindow(WindowHandle, SW_SHOW);
	if ((GLAD_WGL_EXT_swap_control) && (NewConfiguration.VSyncEnabled))
		{
		wglSwapIntervalEXT(1);
		}
	return GLContext;
OnError:
	if (GLContext)
		{
		wglMakeCurrent(NULL, GLContext);
		wglDeleteContext(GLContext);
		GLContext = NULL;
		}
	if (WindowDC)
		{
		ReleaseDC(WindowHandle, WindowDC);
		WindowDC = NULL;
		}
	if (FakeContext)
		{
		wglDeleteContext(FakeContext);
		FakeContext = NULL;
		}
	if (FakeDC)
		{
		ReleaseDC(FakeWND, FakeDC);
		FakeDC = NULL;
		}
	if (FakeWND)
		{
		DestroyWindow(FakeWND);
		FakeWND = NULL;
		}

	return NULL;
	}

bool crWGLDeleteContext(void)
	{
	wglMakeCurrent(NULL, GLContext);
	wglDeleteContext(GLContext);
	GLContext = NULL;
	return true;
	}

bool crWGLMakeContextActive(const crWindowHandle WindowHandle)
	{
	HDC WindowDC = GetWindowDC(WindowHandle);
	wglMakeCurrent(WindowDC, GLContext);
	return true;
	}

void crWGLSwapWindowBuffer(const crWindowHandle WindowHandle)
	{
	HDC WindowDC = GetWindowDC(WindowHandle);
	SwapBuffers(WindowDC);
	}