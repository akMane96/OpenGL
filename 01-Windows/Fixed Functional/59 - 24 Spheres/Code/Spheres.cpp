//header files
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include<stdio.h>

#include "icon.h"

//Macros
#define  ASM_WIN_WIDTH 800
#define ASM_WIN_HEIGHT 600

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

bool ASM_gbLight = false;
GLfloat ASM_light_model_ambient[] = { 0.2f,0.2f,0.2f,1.0f };
GLfloat ASM_light_model_local_viewer[] = {0.0f};

GLfloat ASM_lightAngleX = 0.0f;
GLfloat ASM_lightAngleY = 0.0f;
GLfloat ASM_lightAngleZ = 0.0f;

int ASM_KeyPressed = 0;

GLfloat ASM_lightAmbient[] = {0.0f,0.0f,0.0f,1.0f};
GLfloat ASM_lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat ASM_lightPosition[] = { 0.0f,3.0f,3.0f,0.0f };  //// Directional Light

GLUquadric* ASM_quadric[24];

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
	ASM_wndclass.hInstance = hInstance;

	ASM_wndclass.lpszClassName = ASM_szAppName;
	ASM_wndclass.lpszMenuName = NULL;
	ASM_wndclass.lpfnWndProc = WndProc;


	//Register Class to OS
	RegisterClassEx(&ASM_wndclass);


	//Create Window
	ASM_hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		ASM_szAppName,
		TEXT("Akshay 24 Spheres OpenGL"),
		WS_OVERLAPPEDWINDOW,
		100,
		100,
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

	///
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

	case WM_CHAR:
		switch (wParam)
		{
			case 'l':
			case 'L':
				if (ASM_gbLight == true)
				{
					ASM_gbLight = false;
				}
				else if (ASM_gbLight == false)
				{
					ASM_gbLight = true;
				}

				if (ASM_gbLight == true)
				{
					glEnable(GL_LIGHTING);
				}
				else
				{
					glDisable(GL_LIGHTING);
				}
				break;

			case 'x':
			case 'X':
				ASM_KeyPressed = 1;
				ASM_lightAngleX = 0.0f;				
				break;

			case 'y':
			case 'Y':
				ASM_KeyPressed = 2;
				ASM_lightAngleY = 0.0f;
				break;

			case 'z':
			case 'Z':
				ASM_KeyPressed = 3;
				ASM_lightAngleZ = 0.0f;				
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

	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	//Depth
	glShadeModel(GL_SMOOTH);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
	
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ASM_light_model_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, ASM_light_model_local_viewer);

	////Light0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ASM_lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, ASM_lightDiffuse);
	

	glEnable(GL_LIGHT0);

	for (int ASM_i = 0; ASM_i < 24; ASM_i++)
	{
		ASM_quadric[ASM_i] = gluNewQuadric();
	}
		

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

	//gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	if (height > width)
	{
		glOrtho(0.0f, 15.5f, 0.0f, 15.5f * ((GLfloat)height / (GLfloat)width), -10.0f, 10.0f);
	}else
	{
		glOrtho(0.0f, 15.5f * ((GLfloat)width/ (GLfloat)height), 0.0f, 15.5f , -10.0f, 10.0f);
	}
	

}

void Display()
{
	void update();
	void Draw24Spheres(void);

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (ASM_KeyPressed == 1)
	{
		glRotatef(ASM_lightAngleX, 1.0f, 0.0f, 0.0f);
		ASM_lightPosition[1] = ASM_lightAngleX;

	}
	else if (ASM_KeyPressed == 2)
	{
		glRotatef(ASM_lightAngleY, 0.0f, 1.0f, 0.0f);
		ASM_lightPosition[2] = ASM_lightAngleY;

	}
	else if (ASM_KeyPressed == 3)
	{
		glRotatef(ASM_lightAngleZ, 0.0f, 0.0f, 1.0f);
		ASM_lightPosition[0] = ASM_lightAngleZ;

	}

	glLightfv(GL_LIGHT0, GL_POSITION, ASM_lightPosition);
	Draw24Spheres();
		
	update();

	SwapBuffers(ASM_ghdc);

}

void update()
{

	if (ASM_KeyPressed == 1)
	{
		ASM_lightAngleX = ASM_lightAngleX + 1.0f;

	}
	else if (ASM_KeyPressed == 2)
	{
		ASM_lightAngleY = ASM_lightAngleY + 1.0f;

	}
	else if (ASM_KeyPressed == 3)
	{
		ASM_lightAngleZ = ASM_lightAngleZ + 1.0f;

	}
	
}

void Draw24Spheres()
{

	GLfloat ASM_materialAmbient[4];
	GLfloat ASM_materialDiffuse[4] ;
	GLfloat ASM_materialSpecular[4];
	GLfloat ASM_materialShininess;

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//////////////////////////////////////////////////////***** 1st sphere on 1st column, emerald *****
	ASM_materialAmbient[0] = 0.0215;
	ASM_materialAmbient[1] = 0.1745;
	ASM_materialAmbient[2] = 0.0215;
	ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.07568;
	ASM_materialDiffuse[1] = 0.61424;
	ASM_materialDiffuse[2] = 0.07568;
	ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.633;
	ASM_materialSpecular[1] = 0.727811;
	ASM_materialSpecular[2] = 0.633;
	ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.6 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.5f,14.0f, 0.0f);

	gluSphere(ASM_quadric[0], 1.0f, 30, 30);

	////////////////////////////////////////////////////// ***** 2nd sphere on 1st column, jade *****
	ASM_materialAmbient[0] = 0.135;
	ASM_materialAmbient[1] = 0.2225;
	ASM_materialAmbient[2] = 0.1575;
	ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.54;
	ASM_materialDiffuse[1] = 0.89;
	ASM_materialDiffuse[2] = 0.63;
	ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.316228;
	ASM_materialSpecular[1] = 0.316228;
	ASM_materialSpecular[2] = 0.316228;
	ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.1 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.5f, 11.5f, 0.0f);

	gluSphere(ASM_quadric[1], 1.0f, 30, 30);

	////////////////////////////////////////////////////// ***** 3rd sphere on 1st column, obsidian *****
	ASM_materialAmbient[0] = 0.05375;
	ASM_materialAmbient[1] = 0.05;
	ASM_materialAmbient[2] = 0.06625;
	ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.18275;
	ASM_materialDiffuse[1] = 0.17;
	ASM_materialDiffuse[2] = 0.22525;
	ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.332741;
	ASM_materialSpecular[1] = 0.328634;
	ASM_materialSpecular[2] = 0.346435;
	ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.3 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.5f, 9.0f, 0.0f);

	gluSphere(ASM_quadric[2], 1.0f, 30, 30);

	////////////////////////////////////////////////////// ***** 4th sphere on 1st column, pearl *****
	ASM_materialAmbient[0] = 0.25;
	ASM_materialAmbient[1] = 0.20725;
	ASM_materialAmbient[2] = 0.20725;
	ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 1.0;
	ASM_materialDiffuse[1] = 0.829;
	ASM_materialDiffuse[2] = 0.829;
	ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.296648;
	ASM_materialSpecular[1] = 0.296648;
	ASM_materialSpecular[2] = 0.296648;
	ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.088 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.5f, 6.5f, 0.0f);

	gluSphere(ASM_quadric[3], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 5th sphere on 1st column, ruby *****
	ASM_materialAmbient[0] = 0.1745;
	ASM_materialAmbient[1] = 0.01175;
	ASM_materialAmbient[2] = 0.01175;
	ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.61424;
	ASM_materialDiffuse[1] = 0.04136;
	ASM_materialDiffuse[2] = 0.04136;
	ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.727811;
	ASM_materialSpecular[1] = 0.626959;
	ASM_materialSpecular[2] = 0.626959;
	ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.6 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.5f, 4.0f, 0.0f);

	gluSphere(ASM_quadric[4], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 6th sphere on 1st column, turquoise *****
	ASM_materialAmbient[0] = 0.1;
	ASM_materialAmbient[1] = 0.18725;
	ASM_materialAmbient[2] = 0.1745;
	ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.396;
	ASM_materialDiffuse[1] = 0.74151;
	ASM_materialDiffuse[2] = 0.69102;
	ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.297254;
	ASM_materialSpecular[1] = 0.30829;
	ASM_materialSpecular[2] = 0.306678;
	ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.1 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.5f, 1.5f, 0.0f);

	gluSphere(ASM_quadric[5], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 1st sphere on 2nd column, brass *****
	ASM_materialAmbient[0] = 0.329412;
		ASM_materialAmbient[1] = 0.223529;
		ASM_materialAmbient[2] = 0.027451;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.780392;
		ASM_materialDiffuse[1] = 0.568627;
		ASM_materialDiffuse[2] = 0.113725;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.992157;
		ASM_materialSpecular[1] = 0.941176;
		ASM_materialSpecular[2] = 0.807843;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.21794872 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(9.0f, 14.0f, 0.0f);

	gluSphere(ASM_quadric[6], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 2nd sphere on 2nd column, bronze *****
	ASM_materialAmbient[0] = 0.2125;
		ASM_materialAmbient[1] = 0.1275;
		ASM_materialAmbient[2] = 0.054;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.714;
		ASM_materialDiffuse[1] = 0.4284;
		ASM_materialDiffuse[2] = 0.18144;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.393548;
		ASM_materialSpecular[1] = 0.271906;
		ASM_materialSpecular[2] = 0.166721;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.2 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(9.0f, 11.5f, 0.0f);

	gluSphere(ASM_quadric[7], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 3rd sphere on 2nd column, chrome *****
	ASM_materialAmbient[0] = 0.25;
		ASM_materialAmbient[1] = 0.25;
		ASM_materialAmbient[2] = 0.25;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.4;
		ASM_materialDiffuse[1] = 0.4;
		ASM_materialDiffuse[2] = 0.4;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.774597;
		ASM_materialSpecular[1] = 0.774597;
		ASM_materialSpecular[2] = 0.774597;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.6 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(9.0f, 9.0f, 0.0f);

	gluSphere(ASM_quadric[8], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 4th sphere on 2nd column, copper *****
	ASM_materialAmbient[0] = 0.19125;
		ASM_materialAmbient[1] = 0.0735;
		ASM_materialAmbient[2] = 0.0225;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.7038;
		ASM_materialDiffuse[1] = 0.27048;
		ASM_materialDiffuse[2] = 0.0828;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.256777;
		ASM_materialSpecular[1] = 0.137622;
		ASM_materialSpecular[2] = 0.086014;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.1 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(9.0f, 6.5f, 0.0f);

	gluSphere(ASM_quadric[9], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 5th sphere on 2nd column, gold *****
	ASM_materialAmbient[0] = 0.24725;
		ASM_materialAmbient[1] = 0.1995;
		ASM_materialAmbient[2] = 0.0745;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.75164;
		ASM_materialDiffuse[1] = 0.60648;
		ASM_materialDiffuse[2] = 0.22648;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.628281;
		ASM_materialSpecular[1] = 0.555802;
		ASM_materialSpecular[2] = 0.366065;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.4 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(9.0f, 4.0f, 0.0f);

	gluSphere(ASM_quadric[10], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 6th sphere on 2nd column, silver *****
	ASM_materialAmbient[0] = 0.19225;
		ASM_materialAmbient[1] = 0.19225;
		ASM_materialAmbient[2] = 0.19225;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.50754;
		ASM_materialDiffuse[1] = 0.50754;
		ASM_materialDiffuse[2] = 0.50754;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.508273;
		ASM_materialSpecular[1] = 0.508273;
		ASM_materialSpecular[2] = 0.508273;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.4 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(9.0f, 1.5f, 0.0f);

	gluSphere(ASM_quadric[11], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 1st sphere on 3rd column, black *****
	ASM_materialAmbient[0] = 0.0;
		ASM_materialAmbient[1] = 0.0;
		ASM_materialAmbient[2] = 0.0;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.01;
		ASM_materialDiffuse[1] = 0.01;
		ASM_materialDiffuse[2] = 0.01;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.50;
		ASM_materialSpecular[1] = 0.50;
		ASM_materialSpecular[2] = 0.50;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.25 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(18.0f, 14.0f, 0.0f);

	gluSphere(ASM_quadric[12], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 2nd sphere on 3rd column, cyan *****
	ASM_materialAmbient[0] = 0.0;
		ASM_materialAmbient[1] = 0.1;
		ASM_materialAmbient[2] = 0.06;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.0;
		ASM_materialDiffuse[1] = 0.50980392;
		ASM_materialDiffuse[2] = 0.50980392;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.50196078;
		ASM_materialSpecular[1] = 0.50196078;
		ASM_materialSpecular[2] = 0.50196078;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.25 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(18.0f, 11.5f, 0.0f);

	gluSphere(ASM_quadric[13], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 3rd sphere on 2nd column, green *****
	ASM_materialAmbient[0] = 0.0;
		ASM_materialAmbient[1] = 0.0;
		ASM_materialAmbient[2] = 0.0;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.1;
		ASM_materialDiffuse[1] = 0.35;
		ASM_materialDiffuse[2] = 0.1;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.45;
		ASM_materialSpecular[1] = 0.55;
		ASM_materialSpecular[2] = 0.45;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.25 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(18.0f, 9.0f, 0.0f);

	gluSphere(ASM_quadric[14], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 4th sphere on 3rd column, red *****
	ASM_materialAmbient[0] = 0.0;
		ASM_materialAmbient[1] = 0.0;
		ASM_materialAmbient[2] = 0.0;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.5;
		ASM_materialDiffuse[1] = 0.0;
		ASM_materialDiffuse[2] = 0.0;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.7;
		ASM_materialSpecular[1] = 0.6;
		ASM_materialSpecular[2] = 0.6;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.25 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(18.0f, 6.5f, 0.0f);

	gluSphere(ASM_quadric[15], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 5th sphere on 3rd column, white ****
	ASM_materialAmbient[0] = 0.0;
		ASM_materialAmbient[1] = 0.0;
		ASM_materialAmbient[2] = 0.0;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.55;
		ASM_materialDiffuse[1] = 0.55;
		ASM_materialDiffuse[2] = 0.55;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.70;
		ASM_materialSpecular[1] = 0.70;
		ASM_materialSpecular[2] = 0.70;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.25 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(18.0f, 4.0f, 0.0f);

	gluSphere(ASM_quadric[16], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 6th sphere on 3rd column, yellow plastic *****
	ASM_materialAmbient[0] = 0.0;
		ASM_materialAmbient[1] = 0.0;
		ASM_materialAmbient[2] = 0.0;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.5;
		ASM_materialDiffuse[1] = 0.5;
		ASM_materialDiffuse[2] = 0.0;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.60;
		ASM_materialSpecular[1] = 0.60;
		ASM_materialSpecular[2] = 0.50;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.25 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(18.0f, 1.5f, 0.0f);

	gluSphere(ASM_quadric[17], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 1st sphere on 4th column, black *****
	ASM_materialAmbient[0] = 0.02;
		ASM_materialAmbient[1] = 0.02;
		ASM_materialAmbient[2] = 0.02;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.01;
		ASM_materialDiffuse[1] = 0.01;
		ASM_materialDiffuse[2] = 0.01;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.4;
		ASM_materialSpecular[1] = 0.4;
		ASM_materialSpecular[2] = 0.4;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.078125 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(26.0f, 14.0f, 0.0f);

	gluSphere(ASM_quadric[18], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 2nd sphere on 4th column, cyan *****
	ASM_materialAmbient[0] = 0.0;
		ASM_materialAmbient[1] = 0.05;
		ASM_materialAmbient[2] = 0.05;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.4;
		ASM_materialDiffuse[1] = 0.5;
		ASM_materialDiffuse[2] = 0.5;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.04;
		ASM_materialSpecular[1] = 0.7;
		ASM_materialSpecular[2] = 0.7;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.078125 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(26.0f, 11.5f, 0.0f);

	gluSphere(ASM_quadric[19], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 3rd sphere on 4th column, green *****
	ASM_materialAmbient[0] = 0.0;
		ASM_materialAmbient[1] = 0.05;
		ASM_materialAmbient[2] = 0.0;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.4;
		ASM_materialDiffuse[1] = 0.5;
		ASM_materialDiffuse[2] = 0.4;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.04;
		ASM_materialSpecular[1] = 0.7;
		ASM_materialSpecular[2] = 0.04;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.078125 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(26.0f, 9.0f, 0.0f);

	gluSphere(ASM_quadric[20], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 4th sphere on 4th column, red *****
	ASM_materialAmbient[0] = 0.05;
		ASM_materialAmbient[1] = 0.0;
		ASM_materialAmbient[2] = 0.0;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.5;
		ASM_materialDiffuse[1] = 0.4;
		ASM_materialDiffuse[2] = 0.4;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.7;
		ASM_materialSpecular[1] = 0.04;
		ASM_materialSpecular[2] = 0.04;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.078125 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(26.0f, 6.5f, 0.0f);

	gluSphere(ASM_quadric[21], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 5th sphere on 4th column, white *****
	ASM_materialAmbient[0] = 0.05;
		ASM_materialAmbient[1] = 0.05;
		ASM_materialAmbient[2] = 0.05;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.5;
		ASM_materialDiffuse[1] = 0.5;
		ASM_materialDiffuse[2] = 0.5;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.7;
		ASM_materialSpecular[1] = 0.7;
		ASM_materialSpecular[2] = 0.7;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.078125 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(26.0f, 4.0f, 0.0f);

	gluSphere(ASM_quadric[22], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 6th sphere on 4th column, yellow rubber *****
	ASM_materialAmbient[0] = 0.05;
		ASM_materialAmbient[1] = 0.05;
		ASM_materialAmbient[2] = 0.0;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.5;
		ASM_materialDiffuse[1] = 0.5;
		ASM_materialDiffuse[2] = 0.4;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.7;
		ASM_materialSpecular[1] = 0.7;
		ASM_materialSpecular[2] = 0.04;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.078125 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(26.0f, 1.5f, 0.0f);

	gluSphere(ASM_quadric[23], 1.0f, 30, 30);

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

	for (int ASM_i = 0; ASM_i < 24; ASM_i++)
	{
		if (ASM_quadric[ASM_i])
		{
			gluDeleteQuadric(ASM_quadric[ASM_i]);
			ASM_quadric[ASM_i] = NULL;
		}
	}
	

	if (ASM_gpFile)
	{
		fprintf(ASM_gpFile, "Log Closed Program closed Succssfully !!!!!\n");
		fclose(ASM_gpFile);
		ASM_gpFile = NULL;
	}
}
