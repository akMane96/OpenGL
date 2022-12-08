//header files
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include<stdio.h>
#include "icon.h"

#define _USE_MATH_DEFINES
#include<math.h>

//Macros
#define  ASM_WIN_WIDTH 800
#define ASM_WIN_HEIGHT 600

//Pragma
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
FILE *ASM_gpFile = NULL;
GLfloat ASM_angle;
GLfloat ASM_identityMatrix[16];
GLfloat ASM_translationMatrix[16];
GLfloat ASM_scaleMatrix[16];
GLfloat ASM_rotationMatrix_x[16];
GLfloat ASM_rotationMatrix_y[16];
GLfloat ASM_rotationMatrix_z[16];

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
		TEXT("Akshay Cube OpenGL"),
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

	//Fullscreen on start
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

			switch(wParam)
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
	MONITORINFO ASM_mi = {sizeof(MONITORINFO)};

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
	int ASM_iPixelFormatIndex ;

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
	ASM_pfd.cDepthBits = 32;

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

	


	//Identity matrix
	ASM_identityMatrix[0] = 1.0f;
	ASM_identityMatrix[1] = 0.0f;
	ASM_identityMatrix[2] = 0.0f;
	ASM_identityMatrix[3] = 0.0f;
	ASM_identityMatrix[4] = 0.0f;
	ASM_identityMatrix[5] = 1.0f;
	ASM_identityMatrix[6] = 0.0f;
	ASM_identityMatrix[7] = 0.0f;
	ASM_identityMatrix[8] = 0.0f;
	ASM_identityMatrix[9] = 0.0f;
	ASM_identityMatrix[10] = 1.0f;
	ASM_identityMatrix[11] = 0.0f;
	ASM_identityMatrix[12] = 0.0f;
	ASM_identityMatrix[13] = 0.0f;
	ASM_identityMatrix[14] = 0.0f;
	ASM_identityMatrix[15] = 1.0f;
	

	//Translation matrix
	ASM_translationMatrix[0] = 1.0f;
	ASM_translationMatrix[1] = 0.0f;
	ASM_translationMatrix[2] = 0.0f;
	ASM_translationMatrix[3] = 0.0f;
	ASM_translationMatrix[4] = 0.0f;
	ASM_translationMatrix[5] = 1.0f;
	ASM_translationMatrix[6] = 0.0f;
	ASM_translationMatrix[7] = 0.0f;
	ASM_translationMatrix[8] = 0.0f;
	ASM_translationMatrix[9] = 0.0f;
	ASM_translationMatrix[10] = 1.0f;
	ASM_translationMatrix[11] = 0.0f;
	ASM_translationMatrix[12] = 0.0f;
	ASM_translationMatrix[13] = 0.0f;
	ASM_translationMatrix[14] = -5.0f;
	ASM_translationMatrix[15] = 1.0f;

	//Scale Matrix
	 ASM_scaleMatrix[0] = 0.75f;
	 ASM_scaleMatrix[1] = 0.0f;
	 ASM_scaleMatrix[2] = 0.0f;
	 ASM_scaleMatrix[3] = 0.0f;
	 ASM_scaleMatrix[4] = 0.0f;
	 ASM_scaleMatrix[5] = 0.75f;
	 ASM_scaleMatrix[6] = 0.0f;
	 ASM_scaleMatrix[7] = 0.0f;
	 ASM_scaleMatrix[8] = 0.0f;
	 ASM_scaleMatrix[9] = 0.0f;
	 ASM_scaleMatrix[10] = 0.75f;
	 ASM_scaleMatrix[11] = 0.0f;
	 ASM_scaleMatrix[12] = 0.0f;
	 ASM_scaleMatrix[13] = 0.0f;
	 ASM_scaleMatrix[14] = 0.0f;
	 ASM_scaleMatrix[15] = 1.0f;


	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//Depth
	glShadeModel(GL_SMOOTH);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


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
	void Cube();

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	
	Cube();

	SwapBuffers(ASM_ghdc);

}

void Cube()
{
	//function declaration
	void update();

	//varaiable declaration
	static GLfloat ASM_angleRad = 0.0f;

	glMatrixMode(GL_MODELVIEW);

	/////// Changes 

	////glLoadIdentity();
	glLoadMatrixf(ASM_identityMatrix);

	////glTranslatef(0.0f, 0.0f, -5.0f);
	glMultMatrixf(ASM_translationMatrix);
	
	////glScalef(0.75f, 0.75, 0.75f);
	glMultMatrixf(ASM_scaleMatrix);

	ASM_angleRad = ASM_angle * (M_PI / 180.0f);

	/*GLfloat ASM_identityMatrix[16];
	GLfloat ASM_translationMatrix[16];
	GLfloat ASM_scaleMatrix[16];
	GLfloat ASM_rotationMatrix_x[16];
	GLfloat ASM_rotationMatrix_y[16];
	GLfloat ASM_rotationMatrix_z[16];*/

	//Rotation X Matrix
	////glRotatef(ASM_angle, 1.0f, 0.0f, 0.0f);
	ASM_rotationMatrix_x[0] = 1.0f;
	ASM_rotationMatrix_x[1] = 0.0f;
	ASM_rotationMatrix_x[2] = 0.0f;
	ASM_rotationMatrix_x[3] = 0.0f;
	ASM_rotationMatrix_x[4] = 0.0f;
	ASM_rotationMatrix_x[5] = cos(ASM_angleRad);
	ASM_rotationMatrix_x[6] = sin(ASM_angleRad);
	ASM_rotationMatrix_x[7] = 0.0f;
	ASM_rotationMatrix_x[8] = 0.0f;
	ASM_rotationMatrix_x[9] = -sin(ASM_angleRad);
	ASM_rotationMatrix_x[10] = cos(ASM_angleRad);
	ASM_rotationMatrix_x[11] = 0.0f;
	ASM_rotationMatrix_x[12] = 0.0f;
	ASM_rotationMatrix_x[13] = 0.0f;
	ASM_rotationMatrix_x[14] = 0.0f;
	ASM_rotationMatrix_x[15] = 1.0f;


	//Rotation Y Matrix
	////glRotatef(ASM_angle, 0.0f, 1.0f, 0.0f);
	ASM_rotationMatrix_y[0] = cos(ASM_angleRad);
	ASM_rotationMatrix_y[1] = 0.0f;
	ASM_rotationMatrix_y[2] = -sin(ASM_angleRad);
	ASM_rotationMatrix_y[3] = 0.0f;
	ASM_rotationMatrix_y[4] = 0.0f;
	ASM_rotationMatrix_y[5] = 1.0f;
	ASM_rotationMatrix_y[6] = 0.0f;
	ASM_rotationMatrix_y[7] = 0.0f;
	ASM_rotationMatrix_y[8] = sin(ASM_angleRad);
	ASM_rotationMatrix_y[9] = 0.0f;
	ASM_rotationMatrix_y[10] = cos(ASM_angleRad);
	ASM_rotationMatrix_y[11] = 0.0f;
	ASM_rotationMatrix_y[12] = 0.0f;
	ASM_rotationMatrix_y[13] = 0.0f;
	ASM_rotationMatrix_y[14] = 0.0f;
	ASM_rotationMatrix_y[15] = 1.0f;

	//Rotation Z Matrix
	////glRotatef(ASM_angle, 0.0f, 0.0f, 1.0f);
	ASM_rotationMatrix_z[0] = cos(ASM_angleRad);
	ASM_rotationMatrix_z[1] = sin(ASM_angleRad);
	ASM_rotationMatrix_z[2] = 0.0f;
	ASM_rotationMatrix_z[3] = 0.0f;
	ASM_rotationMatrix_z[4] = -sin(ASM_angleRad);
	ASM_rotationMatrix_z[5] = cos(ASM_angleRad);
	ASM_rotationMatrix_z[6] = 0.0f;
	ASM_rotationMatrix_z[7] = 0.0f;
	ASM_rotationMatrix_z[8] = 0.0f;
	ASM_rotationMatrix_z[9] = 0.0f;
	ASM_rotationMatrix_z[10] = 1.0f;
	ASM_rotationMatrix_z[11] = 0.0f;
	ASM_rotationMatrix_z[12] = 0.0f;
	ASM_rotationMatrix_z[13] = 0.0f;
	ASM_rotationMatrix_z[14] = 0.0f;
	ASM_rotationMatrix_z[15] = 1.0f;

	glMultMatrixf(ASM_rotationMatrix_x);
	glMultMatrixf(ASM_rotationMatrix_y);
	glMultMatrixf(ASM_rotationMatrix_z);


	//front face
	glBegin(GL_QUADS);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
	glEnd();

	//right face
	glBegin(GL_QUADS);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();

	//back face
	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glEnd();

	//right face
	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glEnd();

	//up face
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glEnd();

	//down face
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glEnd();

	update();

	
}

void update()
{
	ASM_angle = ASM_angle + 0.5f;


	if (ASM_angle > 360.0f)
		ASM_angle = 0.0f;
}

void UnInitialize()
{

	if(ASM_gbFullScreen == true)
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
