#include <windows.h>
#include <stdio.h>
#include "OpenGL.h"
#include <gl/gl.h>

//macros
#define ASM_WND_WIDTH 800
#define ASM_WND_HEIGHT 600

//Pragma
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

// Global Function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Varaiable Declaration
DWORD ASM_dwStyle;
WINDOWPLACEMENT ASM_wpPrev = { sizeof(WINDOWPLACEMENT) };
bool ASM_gbFullscreen = false;
bool ASM_gbActiveWindow = false;
HWND ASM_ghwnd = NULL;
HDC ASM_ghdc = NULL;
HGLRC ASM_ghrc;
FILE *ASM_gpFile = NULL;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Local Function Declaration
	void Display(void);
	void Initialize(void);

	//Local Varaiable Declaration
	WNDCLASSEX ASM_wndclass;
	HWND ASM_hwnd;
	MSG ASM_msg;
	TCHAR ASM_szAppName[] = TEXT("ASM WINDOW");
	bool ASM_bDone = false;
	
	//code
	
	if (fopen_s(&ASM_gpFile, "ASMLog.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Cant Create Log File"), TEXT("ERROR"), MB_OK | MB_ICONWARNING);
		exit(0);
	}
	else
	{
		fprintf(ASM_gpFile, "Log Opened Programm started Succssfully !!!!!\n");
	}
	

	//Fill value in WNDCLASSEX structure
	ASM_wndclass.cbSize = sizeof(WNDCLASSEX);
	ASM_wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	ASM_wndclass.cbClsExtra = 0;
	ASM_wndclass.cbWndExtra = 0;
	ASM_wndclass.lpfnWndProc = WndProc;
	ASM_wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ASMICON));
	ASM_wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	ASM_wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	ASM_wndclass.lpszClassName = ASM_szAppName;
	ASM_wndclass.lpszMenuName = NULL;
	ASM_wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(ASMICON));

	//Register Class
	RegisterClassEx(&ASM_wndclass);

	//create window
	ASM_hwnd = CreateWindowEx(WS_EX_APPWINDOW,
							ASM_szAppName,
							TEXT("Akshay Window with Double Buffer Blue Screen Code"),
							WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
							100,
							100,
							ASM_WND_WIDTH,
							ASM_WND_HEIGHT,
							NULL,
							NULL,
							hInstance, 
							NULL);

	//assign local hwnd to global hwnd
	ASM_ghwnd = ASM_hwnd;

	Initialize();

	//show window
	ShowWindow(ASM_hwnd, iCmdShow);

	SetForegroundWindow(ASM_hwnd);
	SetFocus(ASM_hwnd);

	

	while (ASM_bDone == false)
	{
		if (PeekMessage(&ASM_msg, NULL, 0, 0, PM_REMOVE))
		{

			if (ASM_msg.message == WM_QUIT)
			{
				ASM_bDone = true;

			}
			else
			{
				TranslateMessage(&ASM_msg);
				DispatchMessage(&ASM_msg);
			}

		}
		else
		{
			if (ASM_gbActiveWindow == true)
			{
				//Here you should call Display() for OpenGL Rendering
				Display();
			}
		}
	}

	return((int)ASM_msg.wParam);

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//Local Function Declaration
	void ToogleFullScreen(void);
	void Resize(int,int);
	void Uninitialize(void);
	
	
	//code

	switch (iMsg)
	{
		case WM_SETFOCUS:
			ASM_gbActiveWindow = true;
			break;

		case WM_KILLFOCUS:
			ASM_gbActiveWindow = false;
			break;

		case WM_ERASEBKGND:
			return(0);

		case WM_SIZE:
			Resize(LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_KEYDOWN:
			
			switch (wParam)
			{
				case VK_ESCAPE:					
					DestroyWindow(hwnd);
					break;

				case 0X46:					
				case 0X66:
						ToogleFullScreen();
						break;
				default:
					break;
			}
			break;

		case WM_CLOSE:
			
			DestroyWindow(hwnd);
			break;

		case WM_DESTROY:
			Uninitialize();
			PostQuitMessage(0);
			break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));

}

void ToogleFullScreen(void)
{
	//Local Varaiable Declaration
	MONITORINFO ASM_mi = { sizeof(MONITORINFO) };

	//code
	if (ASM_gbFullscreen == false)
	{
		ASM_dwStyle = GetWindowLong(ASM_ghwnd, GWL_STYLE);
		
		if (ASM_dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ASM_ghwnd, &ASM_wpPrev) &&
				GetMonitorInfo(MonitorFromWindow(ASM_ghwnd, MONITORINFOF_PRIMARY), &ASM_mi))
			{
				SetWindowLong(ASM_ghwnd, GWL_STYLE, (ASM_dwStyle & ~WS_OVERLAPPEDWINDOW));

				SetWindowPos(ASM_ghwnd,
								HWND_TOP,
								ASM_mi.rcMonitor.left,
								ASM_mi.rcMonitor.top,
								(ASM_mi.rcMonitor.right - ASM_mi.rcMonitor.left),
								(ASM_mi.rcMonitor.bottom - ASM_mi.rcMonitor.top),
								SWP_NOZORDER | SWP_FRAMECHANGED);
				}
		}
		ShowCursor(FALSE);
		ASM_gbFullscreen = true;
		fprintf(ASM_gpFile, "FullScreen Sucess\n");
	}
	else
	{

		SetWindowLong(ASM_ghwnd, GWL_STYLE, (ASM_dwStyle | WS_OVERLAPPEDWINDOW));

		SetWindowPlacement(ASM_ghwnd, &ASM_wpPrev);

		SetWindowPos(ASM_ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
		ASM_gbFullscreen = false;
		fprintf(ASM_gpFile, "FullScreen Unsucess\n");
	}


}

void Initialize(void)
{
	//function declaration
	void Resize(int, int);

	////varaiable declaration
	PIXELFORMATDESCRIPTOR ASM_pfd;
	int ASM_iPixelFormatIndex;


	////code
		ASM_ghdc = GetDC(ASM_ghwnd);	

	ZeroMemory(&ASM_pfd, sizeof(PIXELFORMATDESCRIPTOR));

	ASM_pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	ASM_pfd.nVersion = 1;
	ASM_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	ASM_pfd.iPixelType = PFD_TYPE_RGBA;
	ASM_pfd.cColorBits = 32;
	ASM_pfd.cRedBits = 8;
	ASM_pfd.cGreenBits = 8;
	ASM_pfd.cBlueBits = 8;
	ASM_pfd.cAlphaBits = 8;

	ASM_iPixelFormatIndex = ChoosePixelFormat(ASM_ghdc, &ASM_pfd);

	if (ASM_iPixelFormatIndex == 0)
	{
		
		fprintf(ASM_gpFile, "ChoosePixelFormat() Failed !!!!!\n");
		DestroyWindow(ASM_ghwnd);
	}

	if (SetPixelFormat(ASM_ghdc, ASM_iPixelFormatIndex, &ASM_pfd) == FALSE)
	{
		fprintf(ASM_gpFile, "SetPixelFormat() Failed !!!!!\n");
		DestroyWindow(ASM_ghwnd);
	}

	ASM_ghrc = wglCreateContext(ASM_ghdc);

	if (ASM_ghrc == NULL)
	{			
		fprintf(ASM_gpFile, "wglCreateContext() Fialed !!!!!\n");
		DestroyWindow(ASM_ghwnd);
	}


	if (wglMakeCurrent(ASM_ghdc, ASM_ghrc) == FALSE)
	{
		fprintf(ASM_gpFile, "wglMakeCurrent() Failed !!!!!\n");
		DestroyWindow(ASM_ghwnd);
	}

	//Set Clear Color
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	Resize(ASM_WND_WIDTH, ASM_WND_HEIGHT);

	//warmup Resize

}

void Resize(int width, int height)
{

	//code
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

}
void Display()
{	
	//code
	glClear(GL_COLOR_BUFFER_BIT);

	SwapBuffers(ASM_ghdc);

}

void Uninitialize()
{
	//code
	if (ASM_gbFullscreen == true)
	{
		ASM_dwStyle = GetWindowLong(ASM_ghwnd, GWL_STYLE);

		SetWindowLong(ASM_ghwnd, GWL_STYLE, (ASM_dwStyle | WS_OVERLAPPEDWINDOW));

		SetWindowPlacement(ASM_ghwnd, &ASM_wpPrev);

		SetWindowPos(ASM_ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}

	if (wglGetCurrentContext() == ASM_ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ASM_ghrc)
	{
		wglDeleteContext(ASM_ghrc);
		ASM_ghrc = NULL;
	}

	if (ASM_ghdc)
	{
		ReleaseDC(ASM_ghwnd,ASM_ghdc);
		ASM_ghdc = NULL;
	}


	if (ASM_gpFile)
	{
		fprintf(ASM_gpFile, "Log Closed Program closed Succssfully !!!!!\n");
		fclose(ASM_gpFile);
		ASM_gpFile = NULL;
	}
}

