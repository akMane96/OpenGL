//header files
#include <windows.h>
#include<stdio.h>

#include<gl/glew.h>
#include <gl/gl.h>

#include "icon.h"
#include "vmath.h"
#include<math.h>

//Macros
#define  ASM_WIN_WIDTH 800
#define ASM_WIN_HEIGHT 600
#define M_PI 3.14159265358979323846

using namespace vmath;

//Pragma
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

enum
{
	ASM_ATTRIBUTE_POSITION = 0,
	ASM_ATTRIBUTE_COLOR,
	ASM_ATTRIBUTE_NORMAL,
	ASM_ATTRIBUTE_TEXTURECOORD,

};


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

GLuint ASM_vertexShaderObject;
GLuint ASM_fragmentShaderObject;
GLuint ASM_shaderProgramObject;

GLuint ASM_vaoVertical;
GLuint ASM_vaoHorizontal;
GLuint ASM_vaoVerticalGreen;
GLuint ASM_vaoHorizontalRed;
GLuint ASM_vbo_PositionVertical;
GLuint ASM_vbo_PositionHorizontal;

GLuint ASM_vaoPoint;
GLuint ASM_vboPostionPoint;
GLuint ASM_vboColorPoint;

GLuint ASM_vaoRect;
GLuint ASM_vboPostionRect;
GLuint ASM_vboColorRect;

GLuint ASM_vaoTriangle;
GLuint ASM_vboPostionTriangle;
GLuint ASM_vboColorTriangle;

GLuint ASM_vbo_Color;
GLuint ASM_mvpUniform;

mat4 ASM_perspectiveProjectionMatrix;


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
		TEXT("Coloured Triangle"),
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

	GLenum ASM_glew_error = glewInit();

	if (ASM_glew_error != GLEW_OK)
	{
		fprintf(ASM_gpFile, "glewInit(): Failed !!!!!\n");
		DestroyWindow(ASM_ghwnd);

	}


	////-----------------------------Vertex Shader

	//Create Shader
	ASM_vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide source code

	const GLchar *ASM_vertexShaderSourceCode = 
		"#version 450 core" \
		"\n" \
		"in vec4 ASM_vPosition;" \
		"in vec4 ASM_vColor;" \
		"uniform mat4 ASM_u_mvpMatrix;" \
		"out vec4 ASM_out_Color;" \
		"void main(void)" \
		"{" \
			"gl_Position = ASM_u_mvpMatrix * ASM_vPosition;" \
			"ASM_out_Color = ASM_vColor;" \
		"}";

	glShaderSource(ASM_vertexShaderObject, 1, (const GLchar **)&ASM_vertexShaderSourceCode, NULL);

		//compile Shader
	glCompileShader(ASM_vertexShaderObject);

	GLint ASM_iInfoLogLength = 0;
	GLint ASM_iShaderCompliedStatus = 0;
	char* ASM_szInfoLog = NULL;

	glGetShaderiv(ASM_vertexShaderObject, GL_COMPILE_STATUS, &ASM_iShaderCompliedStatus);
	
	if (ASM_iShaderCompliedStatus == GL_FALSE)
	{
		glGetShaderiv(ASM_vertexShaderObject, GL_INFO_LOG_LENGTH, &ASM_iInfoLogLength);

		if (ASM_iInfoLogLength > 0)
		{
			ASM_szInfoLog = (char*)malloc(sizeof(char)* ASM_iInfoLogLength);
			if (ASM_szInfoLog != NULL)
			{
				GLsizei ASM_written;
				glGetShaderInfoLog(ASM_vertexShaderObject, ASM_iInfoLogLength, &ASM_written, ASM_szInfoLog);
				fprintf(ASM_gpFile, "Vertex Shader Compilation Log : %s\n", ASM_szInfoLog);
				free(ASM_szInfoLog);
				DestroyWindow(ASM_ghwnd);
			}

		}

	}
		////-----------------------------Fragment Shader

	ASM_fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar *ASM_fragmentShaderSourceCode = 
		"#version 450 core" \
		"\n" \
		"in vec4 ASM_out_Color;" \
		"out vec4 ASM_fragColor;" \
		"void main(void)" \
		"{" \
			"ASM_fragColor = ASM_out_Color;" \
		"}";


	glShaderSource(ASM_fragmentShaderObject, 1, (const GLchar **)&ASM_fragmentShaderSourceCode,NULL);

	glCompileShader(ASM_fragmentShaderObject);

	ASM_iInfoLogLength = 0;
	ASM_iShaderCompliedStatus = 0;
	ASM_szInfoLog = NULL;

	glGetShaderiv(ASM_fragmentShaderObject, GL_COMPILE_STATUS, &ASM_iShaderCompliedStatus);

	if (ASM_iShaderCompliedStatus == GL_FALSE)
	{
		glGetShaderiv(ASM_fragmentShaderObject, GL_INFO_LOG_LENGTH, &ASM_iInfoLogLength);

		if (ASM_iInfoLogLength > 0)
		{
			ASM_szInfoLog = (char*)malloc(sizeof(char) * ASM_iInfoLogLength);
			if (ASM_szInfoLog != NULL)
			{
				GLsizei ASM_written;
				glGetShaderInfoLog(ASM_fragmentShaderObject, ASM_iInfoLogLength, &ASM_written, ASM_szInfoLog);
				fprintf(ASM_gpFile, "Fragment Shader Compilation Log : %s\n", ASM_szInfoLog);
				free(ASM_szInfoLog);
				DestroyWindow(ASM_ghwnd);
			}

		}

	}

	// -------------------------- Shader Program

	ASM_shaderProgramObject = glCreateProgram();

	glAttachShader(ASM_shaderProgramObject, ASM_vertexShaderObject);

	glAttachShader(ASM_shaderProgramObject, ASM_fragmentShaderObject);

	glBindAttribLocation(ASM_shaderProgramObject, ASM_ATTRIBUTE_POSITION, "ASM_vPosition");
	glBindAttribLocation(ASM_shaderProgramObject, ASM_ATTRIBUTE_COLOR, "ASM_vColor");
	

	glLinkProgram(ASM_shaderProgramObject);

	ASM_iInfoLogLength = 0;
	ASM_szInfoLog = NULL;
	GLint ASM_iShaderProgramLinkStatus = 0;

	glGetShaderiv(ASM_shaderProgramObject, GL_LINK_STATUS, &ASM_iShaderProgramLinkStatus);

	if (ASM_iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetShaderiv(ASM_shaderProgramObject, GL_INFO_LOG_LENGTH, &ASM_iInfoLogLength);

		if (ASM_iInfoLogLength > 0)
		{
			ASM_szInfoLog = (char*)malloc(sizeof(char) * ASM_iInfoLogLength);
			if (ASM_szInfoLog != NULL)
			{
				GLsizei ASM_written;
				glGetShaderInfoLog(ASM_shaderProgramObject, ASM_iInfoLogLength, &ASM_written, ASM_szInfoLog);
				fprintf(ASM_gpFile, "Shader Program Link Log : %s\n", ASM_szInfoLog);
				free(ASM_szInfoLog);
				DestroyWindow(ASM_ghwnd);
			}

		}

	}

	ASM_mvpUniform = glGetUniformLocation(ASM_shaderProgramObject, "ASM_u_mvpMatrix");
	
	const GLfloat ASM_lineVerticesHorizontal[] =
	{
		4.0f, 0.0f , 0.0f,
		-4.0f, 0.0f, 0.0
	};

	const GLfloat ASM_lineVerticesVertical[] =
	{
		0.0f, 4.0f , 0.0f,
		0.0f, -4.0f, 0.0
	};

	const GLfloat ASM_lineColors[] =
	{
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
	};

	const GLfloat ASM_lineRedColors[] =
	{
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
	};

	const GLfloat ASM_lineGreenColors[] =
	{
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
	};

	const GLfloat ASM_Point[] =
	{
		0.0f, 0.0f, 0.0f		
	};

	const GLfloat ASM_PointColor[] =
	{
		1.0f, 1.0f, 0.0f
	};

	GLfloat ASM_width, ASM_height;

	ASM_width = 8.0f / (2 * sqrtf(2.0f));
	ASM_height = 8.0f / (2 * sqrtf(2.0f));

	const GLfloat ASM_rectVeticies[] =
	{
		ASM_width, ASM_height , 0.0f,
		-ASM_width, ASM_height , 0.0f,
		-ASM_width, ASM_height , 0.0f,
		-ASM_width, -ASM_height , 0.0f,
		-ASM_width, -ASM_height , 0.0f,
		ASM_width, -ASM_height , 0.0f,
		ASM_width, -ASM_height , 0.0f,
		ASM_width, ASM_height , 0.0f

	};

	const GLfloat ASM_rectColor[] =
	{
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};

	const GLfloat ASM_triangleVeticies[] =
	{
		0.0f, ASM_height, 0.0f,
		-ASM_width, -ASM_height,0.0f,
		-ASM_width, -ASM_height,0.0f,
		ASM_width, -ASM_height,0.0f,
		ASM_width, -ASM_height,0.0f,
		0.0f, ASM_height, 0.0f

	};

	const GLfloat ASM_triangleColor[] =
	{
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};



	//////------------------------------------------------------------ Horizontal Blue
	glGenVertexArrays(1, &ASM_vaoHorizontal);
	glBindVertexArray(ASM_vaoHorizontal);

		glGenBuffers(1, &ASM_vbo_PositionHorizontal);
		glBindBuffer(GL_ARRAY_BUFFER, ASM_vbo_PositionHorizontal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_lineVerticesHorizontal), ASM_lineVerticesHorizontal, GL_STATIC_DRAW);
	
		glVertexAttribPointer(ASM_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(ASM_ATTRIBUTE_POSITION);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &ASM_vbo_Color);
		glBindBuffer(GL_ARRAY_BUFFER, ASM_vbo_Color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_lineColors), ASM_lineColors, GL_STATIC_DRAW);

		glVertexAttribPointer(ASM_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(ASM_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//////------------------------------------------------------------ Horizontal Red
	glGenVertexArrays(1, &ASM_vaoHorizontalRed);
	glBindVertexArray(ASM_vaoHorizontalRed);

		glGenBuffers(1, &ASM_vbo_PositionHorizontal);
		glBindBuffer(GL_ARRAY_BUFFER, ASM_vbo_PositionHorizontal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_lineVerticesHorizontal), ASM_lineVerticesHorizontal, GL_STATIC_DRAW);

		glVertexAttribPointer(ASM_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(ASM_ATTRIBUTE_POSITION);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &ASM_vbo_Color);
		glBindBuffer(GL_ARRAY_BUFFER, ASM_vbo_Color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_lineRedColors), ASM_lineRedColors, GL_STATIC_DRAW);

		glVertexAttribPointer(ASM_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(ASM_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//////------------------------------------------------------------ Vertical Blue
	glGenVertexArrays(1, &ASM_vaoVertical);
	glBindVertexArray(ASM_vaoVertical);

			glGenBuffers(1, &ASM_vbo_PositionVertical);
			glBindBuffer(GL_ARRAY_BUFFER, ASM_vbo_PositionVertical);
			glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_lineVerticesVertical), ASM_lineVerticesVertical, GL_STATIC_DRAW);

			glVertexAttribPointer(ASM_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			glEnableVertexAttribArray(ASM_ATTRIBUTE_POSITION);

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenBuffers(1, &ASM_vbo_Color);
			glBindBuffer(GL_ARRAY_BUFFER, ASM_vbo_Color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_lineColors), ASM_lineColors, GL_STATIC_DRAW);

			glVertexAttribPointer(ASM_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(ASM_ATTRIBUTE_COLOR);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		//////------------------------------------------------------------ Vertical Green
		glGenVertexArrays(1, &ASM_vaoVerticalGreen);
		glBindVertexArray(ASM_vaoVerticalGreen);

			glGenBuffers(1, &ASM_vbo_PositionVertical);
			glBindBuffer(GL_ARRAY_BUFFER, ASM_vbo_PositionVertical);
			glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_lineVerticesVertical), ASM_lineVerticesVertical, GL_STATIC_DRAW);

			glVertexAttribPointer(ASM_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			glEnableVertexAttribArray(ASM_ATTRIBUTE_POSITION);

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenBuffers(1, &ASM_vbo_Color);
			glBindBuffer(GL_ARRAY_BUFFER, ASM_vbo_Color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_lineGreenColors), ASM_lineGreenColors, GL_STATIC_DRAW);

			glVertexAttribPointer(ASM_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(ASM_ATTRIBUTE_COLOR);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		
		//////------------------------------------------------------------ Point
		glGenVertexArrays(1, &ASM_vaoPoint);
		glBindVertexArray(ASM_vaoPoint);

			glGenBuffers(1, &ASM_vboPostionPoint);
			glBindBuffer(GL_ARRAY_BUFFER, ASM_vboPostionPoint);
			glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_Point), ASM_Point, GL_STATIC_DRAW);

			glVertexAttribPointer(ASM_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			glEnableVertexAttribArray(ASM_ATTRIBUTE_POSITION);

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenBuffers(1, &ASM_vboColorPoint);
			glBindBuffer(GL_ARRAY_BUFFER, ASM_vboColorPoint);
			glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_PointColor), ASM_PointColor, GL_STATIC_DRAW);

			glVertexAttribPointer(ASM_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(ASM_ATTRIBUTE_COLOR);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		
		//////------------------------------------------------------------ Rectangle
		glGenVertexArrays(1, &ASM_vaoRect);
		glBindVertexArray(ASM_vaoRect);

			glGenBuffers(1, &ASM_vboPostionRect);
			glBindBuffer(GL_ARRAY_BUFFER, ASM_vboPostionRect);
			glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_rectVeticies), ASM_rectVeticies, GL_STATIC_DRAW);

			glVertexAttribPointer(ASM_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			glEnableVertexAttribArray(ASM_ATTRIBUTE_POSITION);

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenBuffers(1, &ASM_vboColorRect);
			glBindBuffer(GL_ARRAY_BUFFER, ASM_vboColorRect);
			glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_rectColor), ASM_rectColor, GL_STATIC_DRAW);

			glVertexAttribPointer(ASM_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(ASM_ATTRIBUTE_COLOR);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		
		//////------------------------------------------------------------ Triangle
		glGenVertexArrays(1, &ASM_vaoTriangle);
		glBindVertexArray(ASM_vaoTriangle);

		glGenBuffers(1, &ASM_vboPostionTriangle);
		glBindBuffer(GL_ARRAY_BUFFER, ASM_vboPostionTriangle);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_triangleVeticies), ASM_triangleVeticies, GL_STATIC_DRAW);

		glVertexAttribPointer(ASM_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(ASM_ATTRIBUTE_POSITION);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &ASM_vboColorTriangle);
		glBindBuffer(GL_ARRAY_BUFFER, ASM_vboColorTriangle);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_triangleColor), ASM_triangleColor, GL_STATIC_DRAW);

		glVertexAttribPointer(ASM_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(ASM_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

	//Depth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);

	ASM_perspectiveProjectionMatrix = mat4::identity();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Graphic Library Shading Language
///// OpenGL Related Log
	fprintf(ASM_gpFile, "\n------------------- OpenGL Related Log ---------------------\n\n");
	fprintf(ASM_gpFile, "OpenGL Vendor : %s\n\n", glGetString(GL_VENDOR));
	fprintf(ASM_gpFile, "OpenGL Renderer : %s\n\n", glGetString(GL_RENDERER));
	fprintf(ASM_gpFile, "OpenGL Version : %s\n\n", glGetString(GL_VERSION));
	fprintf(ASM_gpFile, "GLSL Version : %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	//OpenGL Enabled Extension
	fprintf(ASM_gpFile, "\n------------------- OpenGL Enabled Extension ---------------------\n\n");

	GLint ASM_numExt;

	glGetIntegerv(GL_NUM_EXTENSIONS, &ASM_numExt);

	for (int ASM_i = 0; ASM_i < ASM_numExt; ASM_i++)
	{
		fprintf(ASM_gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, ASM_i));
	}

	fprintf(ASM_gpFile, "\n\n------------------------------------------------------\n\n");

	


	//Warm Up Resize
	Resize(ASM_WIN_WIDTH, ASM_WIN_HEIGHT);
	
}


void Resize(int width, int height)
{
	//code
	if (height <= 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);	

	ASM_perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width/ (GLfloat)height, 0.1f, 100.0f);

}

void Display()
{

	void Circle();
	void Graph();
	void Rectangle();
	void Triangle();
	void Incircle();

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ASM_shaderProgramObject);		


	Circle();
	Graph();
	Rectangle();
	Triangle();
	Incircle();
	glUseProgram(0);

	
	SwapBuffers(ASM_ghdc);

}



void Graph()
{

	mat4 ASM_modelViewMatrix = mat4::identity();
	mat4 ASM_modelViewProjectMatrix = mat4::identity();
	mat4 ASM_translateMatrix = mat4::identity();

	GLfloat ASM_NoOfLines = 20.0f;
	GLfloat ASM_gap = 4.0f / ASM_NoOfLines;

	///////---------------------------------------------------------------------------------Horizontal Lines
	ASM_translateMatrix = translate(0.0f, 0.0f, -11.0f);

	ASM_modelViewMatrix = ASM_translateMatrix;

	ASM_modelViewProjectMatrix = ASM_perspectiveProjectionMatrix * ASM_modelViewMatrix;

	glUniformMatrix4fv(ASM_mvpUniform, 1, GL_FALSE, ASM_modelViewProjectMatrix);

	///////-------------------------------------------------------- Up
	glBindVertexArray(ASM_vaoHorizontalRed);

	glDrawArrays(GL_LINES, 0, 2);

	glBindVertexArray(0);

	glBindVertexArray(ASM_vaoVerticalGreen);

	glDrawArrays(GL_LINES, 0, 2);

	glBindVertexArray(0);

	for (int ASM_i = 0; ASM_i < 20; ASM_i++)
	{

		ASM_translateMatrix = translate(0.0f, ASM_gap, 0.0f);

		ASM_modelViewMatrix = ASM_modelViewMatrix * ASM_translateMatrix;

		ASM_modelViewProjectMatrix = ASM_perspectiveProjectionMatrix * ASM_modelViewMatrix;

		glUniformMatrix4fv(ASM_mvpUniform, 1, GL_FALSE, ASM_modelViewProjectMatrix);

		glBindVertexArray(ASM_vaoHorizontal);

		glDrawArrays(GL_LINES, 0, 2);

		glBindVertexArray(0);


	}

	///////-------------------------------------------------------- Down
	ASM_translateMatrix = mat4::identity();
	ASM_modelViewMatrix = mat4::identity();
	ASM_modelViewProjectMatrix = mat4::identity();

	ASM_translateMatrix = translate(0.0f, 0.0f, -11.0f);
	ASM_modelViewMatrix = ASM_translateMatrix;

	for (int ASM_i = 0; ASM_i < 20; ASM_i++)
	{

		ASM_translateMatrix = translate(0.0f, -ASM_gap, 0.0f);

		ASM_modelViewMatrix = ASM_modelViewMatrix * ASM_translateMatrix;

		ASM_modelViewProjectMatrix = ASM_perspectiveProjectionMatrix * ASM_modelViewMatrix;

		glUniformMatrix4fv(ASM_mvpUniform, 1, GL_FALSE, ASM_modelViewProjectMatrix);

		glBindVertexArray(ASM_vaoHorizontal);

		glDrawArrays(GL_LINES, 0, 2);

		glBindVertexArray(0);


	}

	///////--------------------------------------------------------------------------------- Vertical Lines


	ASM_translateMatrix = mat4::identity();
	ASM_modelViewMatrix = mat4::identity();
	ASM_modelViewProjectMatrix = mat4::identity();

	ASM_translateMatrix = translate(0.0f, 0.0f, -11.0f);
	ASM_modelViewMatrix = ASM_translateMatrix;



	///////-------------------------------------------------------- Right
	for (int ASM_i = 0; ASM_i < 20; ASM_i++)
	{

		ASM_translateMatrix = translate(ASM_gap, 0.0f, 0.0f);

		ASM_modelViewMatrix = ASM_modelViewMatrix * ASM_translateMatrix;

		ASM_modelViewProjectMatrix = ASM_perspectiveProjectionMatrix * ASM_modelViewMatrix;

		glUniformMatrix4fv(ASM_mvpUniform, 1, GL_FALSE, ASM_modelViewProjectMatrix);

		glBindVertexArray(ASM_vaoVertical);

		glDrawArrays(GL_LINES, 0, 2);

		glBindVertexArray(0);


	}


	///////-------------------------------------------------------- Left
	ASM_translateMatrix = mat4::identity();
	ASM_modelViewMatrix = mat4::identity();
	ASM_modelViewProjectMatrix = mat4::identity();

	ASM_translateMatrix = translate(0.0f, 0.0f, -11.0f);
	ASM_modelViewMatrix = ASM_translateMatrix;



	for (int ASM_i = 0; ASM_i < 20; ASM_i++)
	{

		ASM_translateMatrix = translate(-ASM_gap, 0.0f, 0.0f);

		ASM_modelViewMatrix = ASM_modelViewMatrix * ASM_translateMatrix;

		ASM_modelViewProjectMatrix = ASM_perspectiveProjectionMatrix * ASM_modelViewMatrix;

		glUniformMatrix4fv(ASM_mvpUniform, 1, GL_FALSE, ASM_modelViewProjectMatrix);

		glBindVertexArray(ASM_vaoVertical);

		glDrawArrays(GL_LINES, 0, 2);

		glBindVertexArray(0);


	}
	
	
}


void Circle()
{
	mat4 ASM_translateMatrix = mat4::identity();
	mat4 ASM_modelViewMatrix = mat4::identity();
	mat4 ASM_modelViewProjectMatrix = mat4::identity();

	ASM_translateMatrix = translate(-4.0f, 0.0f, -11.0f);
	ASM_modelViewMatrix = ASM_translateMatrix;
	ASM_modelViewProjectMatrix = ASM_perspectiveProjectionMatrix * ASM_modelViewMatrix;

	GLfloat ASM_x, ASM_y;

	glUniformMatrix4fv(ASM_mvpUniform, 1, GL_FALSE, ASM_modelViewProjectMatrix);

	glPointSize(2.0f);

	for (GLfloat ASM_angle = 0.0f; ASM_angle <= (2.0f * M_PI); (ASM_angle = ASM_angle + 0.001f))
	{

		ASM_x = 0.004 * sin(ASM_angle);
		ASM_y = 0.004 * cos(ASM_angle);

		ASM_translateMatrix = translate(ASM_x , ASM_y, 0.0f);
		ASM_modelViewMatrix = ASM_modelViewMatrix * ASM_translateMatrix;
		ASM_modelViewProjectMatrix = ASM_perspectiveProjectionMatrix * ASM_modelViewMatrix;

		glUniformMatrix4fv(ASM_mvpUniform, 1, GL_FALSE, ASM_modelViewProjectMatrix);

		glBindVertexArray(ASM_vaoPoint);

			glDrawArrays(GL_POINTS, 0, 1);

		glBindVertexArray(0);
		
		
	}

}

void Rectangle()
{
	
	mat4 ASM_translateMatrix = mat4::identity();
	mat4 ASM_modelViewMatrix = mat4::identity();
	mat4 ASM_modelViewProjectMatrix = mat4::identity();

	ASM_translateMatrix = translate(0.0f, 0.0f, -11.0f);
	ASM_modelViewMatrix = ASM_translateMatrix;
	ASM_modelViewProjectMatrix = ASM_perspectiveProjectionMatrix * ASM_modelViewMatrix;

	glUniformMatrix4fv(ASM_mvpUniform, 1, GL_FALSE, ASM_modelViewProjectMatrix);

	glBindVertexArray(ASM_vaoRect);

		glDrawArrays(GL_LINES, 0, 8);

	glBindVertexArray(0);

}

void Triangle()
{

	mat4 ASM_translateMatrix = mat4::identity();
	mat4 ASM_modelViewMatrix = mat4::identity();
	mat4 ASM_modelViewProjectMatrix = mat4::identity();

	ASM_translateMatrix = translate(0.0f, 0.0f, -11.0f);
	ASM_modelViewMatrix = ASM_translateMatrix;
	ASM_modelViewProjectMatrix = ASM_perspectiveProjectionMatrix * ASM_modelViewMatrix;

	glUniformMatrix4fv(ASM_mvpUniform, 1, GL_FALSE, ASM_modelViewProjectMatrix);

	glBindVertexArray(ASM_vaoTriangle);

	glDrawArrays(GL_LINES, 0, 6);

	glBindVertexArray(0);

}

void Incircle()
{
	//Variable Declaration
	mat4 ASM_translateMatrix = mat4::identity();
	mat4 ASM_modelViewMatrix = mat4::identity();
	mat4 ASM_modelViewProjectMatrix = mat4::identity();
	GLfloat ASM_angle, ASM_XCenter, ASM_YCenter, ASM_height, ASM_width, ASM_RaidusIncircle, ASM_BSide, ASM_ASide, ASM_CSide;


	//Calculate Height of Triangle 
	ASM_height = 8.0f / (sqrt(2.0f));

	//Calculate Width of Triangle 
	ASM_width = 8.0f / (sqrt(2.0f));

	GLfloat ASM_XTriangle[3] = { 0.0f, -(ASM_width / 2), (ASM_width / 2) };
	GLfloat ASM_YTriangle[3] = { ASM_height / 2 , -(ASM_height / 2), -(ASM_height / 2) };

	//Caluclate length of each side	
	ASM_ASide = sqrt(pow(-(ASM_width / 2) - 0.0f, 2) + pow(-(ASM_height / 2) - (ASM_height / 2), 2));
	ASM_BSide = sqrt(pow((ASM_width / 2) + (ASM_width / 2), 2) + pow(-(ASM_height / 2) + (ASM_height / 2), 2));
	ASM_CSide = ASM_ASide;

	ASM_XCenter = ((ASM_BSide * ASM_XTriangle[0]) + (ASM_CSide * ASM_XTriangle[1]) + (ASM_ASide * ASM_XTriangle[2])) / (ASM_ASide + ASM_BSide + ASM_CSide);
	ASM_YCenter = ((ASM_BSide * ASM_YTriangle[0]) + (ASM_CSide * ASM_YTriangle[1]) + (ASM_ASide * ASM_YTriangle[2])) / (ASM_ASide + ASM_BSide + ASM_CSide);

	ASM_RaidusIncircle = ((ASM_BSide) / 2.0f) * (sqrt((2.0f * ASM_ASide - (ASM_BSide)) / (2.0f * ASM_ASide + (ASM_BSide))));


	//Move to incenter of Triangle 
	//glTranslatef(ASM_XCenter, ASM_YCenter, 0.0f);

	fprintf(ASM_gpFile, "X : %f\n", ASM_XCenter);
	fprintf(ASM_gpFile, "Y : %f\n", ASM_YCenter);

	ASM_translateMatrix = translate(ASM_XCenter - ASM_RaidusIncircle, ASM_YCenter, -11.0f);
	ASM_modelViewMatrix = ASM_translateMatrix;
	ASM_modelViewProjectMatrix = ASM_perspectiveProjectionMatrix * ASM_modelViewMatrix;

	GLfloat ASM_x, ASM_y;

	glUniformMatrix4fv(ASM_mvpUniform, 1, GL_FALSE, ASM_modelViewProjectMatrix);

	glPointSize(2.0f);


	
	for (GLfloat ASM_angle = 0.0f; ASM_angle <= (2.0f * M_PI); (ASM_angle = ASM_angle + 0.01f))
	{

		ASM_x = ASM_RaidusIncircle/100.0 * sin(ASM_angle);
		ASM_y = ASM_RaidusIncircle / 100.0 * cos(ASM_angle);

		ASM_translateMatrix = translate(ASM_x, ASM_y, 0.0f);
		ASM_modelViewMatrix = ASM_modelViewMatrix * ASM_translateMatrix;
		ASM_modelViewProjectMatrix = ASM_perspectiveProjectionMatrix * ASM_modelViewMatrix;

		glUniformMatrix4fv(ASM_mvpUniform, 1, GL_FALSE, ASM_modelViewProjectMatrix);

		glBindVertexArray(ASM_vaoPoint);

		glDrawArrays(GL_POINTS, 0, 1);

		glBindVertexArray(0);


	}


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

	if (ASM_vaoHorizontal)
	{
		glDeleteVertexArrays(1, &ASM_vaoHorizontal);
		ASM_vaoHorizontal = 0;
	}

	if (ASM_vaoVertical)
	{
		glDeleteVertexArrays(1, &ASM_vaoVertical);
		ASM_vaoVertical = 0;
	}

	if (ASM_vaoHorizontalRed)
	{
		glDeleteVertexArrays(1, &ASM_vaoHorizontalRed);
		ASM_vaoHorizontalRed = 0;
	}

	if (ASM_vaoVerticalGreen)
	{
		glDeleteVertexArrays(1, &ASM_vaoVerticalGreen);
		ASM_vaoVerticalGreen = 0;
	}


	if (ASM_vbo_PositionHorizontal)
	{
		glDeleteBuffers(1, &ASM_vbo_PositionHorizontal);
		ASM_vbo_PositionHorizontal = 0;
	}

	if (ASM_vbo_PositionVertical)
	{
		glDeleteBuffers(1, &ASM_vbo_PositionVertical);
		ASM_vbo_PositionVertical = 0;
	}

	if (ASM_vbo_Color)
	{
		glDeleteBuffers(1, &ASM_vbo_Color);
		ASM_vbo_Color = 0;
	}

	
	

	if (ASM_vaoPoint)
	{
		glDeleteVertexArrays(1, &ASM_vaoPoint);
		ASM_vaoPoint = 0;
	}


	if (ASM_vaoRect)
	{
		glDeleteVertexArrays(1, &ASM_vaoRect);
		ASM_vaoRect = 0;
	}


	if (ASM_vaoTriangle)
	{
		glDeleteVertexArrays(1, &ASM_vaoTriangle);
		ASM_vaoTriangle = 0;
	}



	if (ASM_vboPostionPoint)
	{
		glDeleteBuffers(1, &ASM_vboPostionPoint);
		ASM_vboPostionPoint = 0;
	}

	if (ASM_vboColorPoint)
	{
		glDeleteBuffers(1, &ASM_vboColorPoint);
		ASM_vboColorPoint = 0;
	}

	if (ASM_vboPostionRect)
	{
		glDeleteBuffers(1, &ASM_vboPostionRect);
		ASM_vboPostionRect = 0;
	}

	if (ASM_vboColorRect)
	{
		glDeleteBuffers(1, &ASM_vboColorRect);
		ASM_vboColorRect = 0;
	}

	if (ASM_vboPostionTriangle)
	{
		glDeleteBuffers(1, &ASM_vboPostionTriangle);
		ASM_vboPostionTriangle = 0;
	}

	if (ASM_vboColorTriangle)
	{
		glDeleteBuffers(1, &ASM_vboColorTriangle);
		ASM_vboColorTriangle = 0;
	}


	////--------------------------------- Safe Shader Cleanup
	if(ASM_shaderProgramObject)
	{

		glUseProgram(ASM_shaderProgramObject);

		GLsizei ASM_shaderCount;

		glGetProgramiv(ASM_shaderProgramObject, GL_ATTACHED_SHADERS, &ASM_shaderCount);

		GLuint * ASM_pShaders = NULL;

		ASM_pShaders = (GLuint *)malloc(ASM_shaderCount*sizeof(GLuint));

		if(ASM_pShaders == NULL)
		{
			fprintf(ASM_gpFile, "Memory Allocation Failed !!!!!\n");
			exit(0);

		}

		glGetAttachedShaders(ASM_shaderProgramObject, ASM_shaderCount,&ASM_shaderCount, ASM_pShaders);

		for(GLsizei ASM_i = 0; ASM_i < ASM_shaderCount; ASM_i++)
		{

			glDetachShader(ASM_shaderProgramObject, ASM_pShaders[ASM_i]);
			glDeleteShader(ASM_pShaders[ASM_i]);
			ASM_pShaders[ASM_i] = 0;

		}

		free(ASM_pShaders);
		ASM_pShaders = NULL;

		glDeleteProgram(ASM_shaderProgramObject);
		ASM_shaderProgramObject = 0;

		glUseProgram(0);

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
