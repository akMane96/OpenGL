//header files
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include<stdio.h>
#include <math.h>

#include "icon.h"

//Macros
#define  ASM_WIN_WIDTH 800
#define ASM_WIN_HEIGHT 600
#define M_PI 3.14159265358979323846

//PragmaASM_WIN_WIDTH
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib,"glu32.lib")

//Global Function Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global Variable Declaration
DWORD ASM_dwStyle;
WINDOWPLACEMENT ASM_wpPrev = { sizeof(WINDOWPLACEMENT) };
HWND ASM_ghwnd = NULL;
HDC ASM_ghdc = NULL;
HGLRC ASM_ghrc;
bool ASM_gbFullScreen = false;
bool ASM_gbActiveWindow = false;
FILE* ASM_gpFile = NULL;
GLfloat ASM_RAngleTriangle, ASM_RAngleCircle;
GLfloat tXTriangle, tYTriangle;
GLfloat tXCircle, tYCircle;
GLfloat tYLine;

//Win MAin
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Local Function Declaration
	void Display();
	void Initialize();
	void ToggleFullScreen();


	//Local Variable Declaration
	WNDCLASSEX ASM_wndclass;
	HWND ASM_hwnd;
	MSG ASM_msg;
	TCHAR ASM_szAppName[] = TEXT("ASM Window");
	bool ASM_iDone = false;
	
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

	//Fill WNDCLASSEX struct
	ASM_wndclass.cbSize = sizeof(WNDCLASSEX);
	ASM_wndclass.cbClsExtra = 0;
	ASM_wndclass.cbWndExtra = 0;
	ASM_wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	ASM_wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ASMICON));
	ASM_wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(ASMICON));
	ASM_wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	ASM_wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);


	ASM_wndclass.lpszClassName = ASM_szAppName;
	ASM_wndclass.lpszMenuName = NULL;
	ASM_wndclass.lpfnWndProc = WndProc;


	//Register Class to OS
	RegisterClassEx(&ASM_wndclass);

	//Create Window
	ASM_hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		ASM_szAppName,
		TEXT("Akshay Deathly Hallows Animated OpenGL"),
		WS_OVERLAPPEDWINDOW,
		(GetSystemMetrics(SM_CXSCREEN) / 2) - (ASM_WIN_WIDTH / 2),
		(GetSystemMetrics(SM_CYSCREEN) / 2) - (ASM_WIN_HEIGHT / 2),
		ASM_WIN_WIDTH,
		ASM_WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ASM_ghwnd = ASM_hwnd;

	//Intialize
	Initialize();

	//Fulscreen for start
	ToggleFullScreen();

	//Show Window
	ShowWindow(ASM_hwnd, iCmdShow);

	//Set Focus
	SetForegroundWindow(ASM_hwnd);
	SetFocus(ASM_ghwnd);

	//Game Loop

	while (ASM_iDone == false)
	{
		if (PeekMessage(&ASM_msg, NULL, 0, 0, PM_REMOVE))
		{
			if (ASM_msg.message == WM_QUIT)
			{
				ASM_iDone = true;
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
				Display();
			}
		}
	}

	return((int)ASM_msg.wParam);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//Local Fuction Declaration
	void ToggleFullScreen();
	void Resize(int, int);
	void UnInitialize();
	void Display();

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
			DestroyWindow(ASM_ghwnd);
			break;

		case 0x46:
		case 0x66:
			ToggleFullScreen();
			break;

		default:
			break;
		}

		break;

	case WM_CLOSE:
		DestroyWindow(ASM_ghwnd);
		break;

	case WM_DESTROY:
		UnInitialize();
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullScreen()
{
	MONITORINFO ASM_mi = { sizeof(MONITORINFO) };

	if (ASM_gbFullScreen == false)
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
					SWP_NOZORDER | SWP_FRAMECHANGED
				);

				ShowCursor(FALSE);
				ASM_gbFullScreen = true;
			}


		}
	}
	else
	{
		SetWindowLong(ASM_ghwnd, GWL_STYLE, ASM_dwStyle | WS_OVERLAPPEDWINDOW);

		SetWindowPlacement(ASM_ghwnd, &ASM_wpPrev);

		SetWindowPos(ASM_ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER
		);

		ShowCursor(TRUE);
		ASM_gbFullScreen = false;
	}
}

void Initialize()
{
	//Local Function Declaration
	void Resize(int, int);

	//Local variable Declaration
	PIXELFORMATDESCRIPTOR ASM_pfd;
	int ASM_iPixelFormatIndex;

	//code

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
		fprintf(ASM_gpFile, "wglCreateContext() Failed !!!!!\n");
		DestroyWindow(ASM_ghwnd);
	}

	if (wglMakeCurrent(ASM_ghdc, ASM_ghrc) == FALSE)
	{
		fprintf(ASM_gpFile, "wglMakeCurrent() Failed !!!!!\n");
		DestroyWindow(ASM_ghwnd);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//Warm Up Resize

	Resize(ASM_WIN_WIDTH, ASM_WIN_HEIGHT);

}


void Resize(int width, int height)
{
	//code
	if (height <= 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void Display()
{
	//funation declration
	void DeathlyHallows();
	
	
	//code
	glClear(GL_COLOR_BUFFER_BIT);		

	DeathlyHallows();
	
	SwapBuffers(ASM_ghdc);

}

void DeathlyHallows()
{
	//function prtotype
	void update();

	//variable declaration
	GLfloat ASM_angle, ASM_part;
	GLfloat ASM_RaidusIncircle;
	GLfloat ASM_move;	

	//Assignment
	ASM_RaidusIncircle = 0.0f;
	
	//Calculations
	ASM_part = sqrtf(3.0f) / 3.0f;
	ASM_RaidusIncircle = ASM_part;

	// Triangle Co-dordinate
	GLfloat ASM_XCo[3] = { 0.0f, -1.0f, 1.0f };
	GLfloat ASM_YCo[3] = { 2*ASM_part, -ASM_part, -ASM_part };

	/////////////////////////////////////////////////////////////////    Triangle
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(0.0f, 0.0f, -8.0f);	

	glTranslatef(-5.0f, -5.0f, 0.0f);

	glTranslatef(tXTriangle, tYTriangle, 0.0f);

	glRotatef(ASM_RAngleTriangle, 0.0f, 1.0f, 0.0f);
	
	glLineWidth(4.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(ASM_XCo[0], ASM_YCo[0], 0.0f);
	glVertex3f(ASM_XCo[1], ASM_YCo[1], 0.0f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(ASM_XCo[1], ASM_YCo[1], 0.0f);
	glVertex3f(ASM_XCo[2], ASM_YCo[2], 0.0f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(ASM_XCo[2], ASM_YCo[2], 0.0f);
	glVertex3f(ASM_XCo[0], ASM_YCo[0], 0.0f);
	glEnd();	

	


	///////////////////////////////////////////////////////////////////    InCircle
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	
	glTranslatef(0.0f, 0.0f, -8.0f);

	glTranslatef(5.0f, -5.0f, 0.0f);

	glTranslatef(-tXCircle , tYCircle, 0.0f);

	glRotatef(ASM_RAngleCircle, 0.0f, 1.0f, 0.0f);

	glPointSize(4.0f);
	//Circle
	for (ASM_angle = 0.0f; ASM_angle <= (2 * M_PI); (ASM_angle = ASM_angle + 0.0001f))
	{
		glBegin(GL_POINTS);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(ASM_RaidusIncircle *sin(ASM_angle), ASM_RaidusIncircle *cos(ASM_angle), 0.0f);
		glEnd();
	}

	/////////////////////////////////////////////////////////////////////    Line
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -8.0f);

	glTranslatef(0.0f, sqrtf(50), 0.0f);

	glTranslatef(0.0f, -tYLine, 0.0f);

	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 2 * ASM_part, 0.0f);
	glVertex3f(0.0f, -ASM_part, 0.0f);
	glEnd();


	update();


}

void update()
{
	//Shapes Translate
	if (tXTriangle < 5.0f && tXTriangle < 5.0f)
	{
		tXTriangle = tXTriangle + 0.01f;
		tYTriangle = tYTriangle + 0.01f;
		ASM_RAngleTriangle = ASM_RAngleTriangle + 2.5f;
	}
	else if (tXCircle < 5.0f && tXCircle < 5.0f)
	{
		ASM_RAngleTriangle = 0.0f;
		tXCircle = tXCircle + 0.01f;
		tYCircle = tYCircle + 0.01f;
		ASM_RAngleCircle = ASM_RAngleCircle + 2.5f;
	}
	else if (tYLine < sqrt(50))
	{
		ASM_RAngleCircle = 0.0f;
		tYLine = tYLine + 0.03f;
	}

	
	if (ASM_RAngleTriangle > 360.0f)
	{
		ASM_RAngleTriangle = 0.0f;
	}

	if (ASM_RAngleCircle > 360.0f)
	{
		ASM_RAngleCircle = 0.0f;
	}


}


void UnInitialize()
{

	if (ASM_gbFullScreen == true)
	{
		SetWindowLong(ASM_ghwnd, GWL_STYLE, ASM_dwStyle | WS_OVERLAPPEDWINDOW);

		SetWindowPlacement(ASM_ghwnd, &ASM_wpPrev);

		SetWindowPos(ASM_ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER
		);

		ShowCursor(TRUE);
		ASM_gbFullScreen = false;

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
		ReleaseDC(ASM_ghwnd, ASM_ghdc);
		ASM_ghdc = NULL;
	}

	if (ASM_gpFile)
	{
		fprintf(ASM_gpFile, "Log Closed Program closed Succssfully !!!!!\n");
		fclose(ASM_gpFile);
		ASM_gpFile = NULL;
	}
}
