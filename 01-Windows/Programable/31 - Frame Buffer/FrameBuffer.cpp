//header files
#include <windows.h>
#include<stdio.h>

#include<gl/glew.h>
#include <gl/gl.h>

#include "ogl.h"
#include "vmath.h"

//Macros


#define  ASM_FRAMEBUFFER_WIDTH 200
#define ASM_FRAMEBUFFER_HEIGHT 200

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

GLuint ASM_vertexShaderObjectDefault;
GLuint ASM_fragmentShaderObjectDefault;
GLuint ASM_shaderProgramObjectDefault;

GLuint ASM_vertexShaderObjectFB;
GLuint ASM_fragmentShaderObjectFB;
GLuint ASM_shaderProgramObjectFB;

GLuint ASM_vao_CubeDefault;
GLuint ASM_vbo_CubePositionDefault;
GLuint ASM_vbo_CubeTextureDefault;

GLuint ASM_vao_CubeFB;
GLuint ASM_vbo_CubePositionFB;
GLuint ASM_vbo_CubeColorFB;

GLuint ASM_mvpUniformFB;
GLuint ASM_mvpUniformDefault;

GLuint ASM_textureSamplerUniform;

mat4 ASM_perspectiveProjectionMatrix;

GLfloat ASM_CubeAngle;

GLuint ASM_frameBuffer;
GLuint ASM_texColorBuffer;
GLuint ASM_rbo;

GLint  ASM_WIN_WIDTH = 800;
GLint ASM_WIN_HEIGHT = 600;

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
		TEXT("Two 3D Shapes Texture"),
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
	void createFramebuffer();

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
	ASM_vertexShaderObjectFB = glCreateShader(GL_VERTEX_SHADER);

	//provide source code

	const GLchar* ASM_vertexShaderSourceCodeFB =
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

	glShaderSource(ASM_vertexShaderObjectFB, 1, (const GLchar**)&ASM_vertexShaderSourceCodeFB, NULL);

	//compile Shader
	glCompileShader(ASM_vertexShaderObjectFB);

	GLint ASM_iInfoLogLength = 0;
	GLint ASM_iShaderCompliedStatus = 0;
	char* ASM_szInfoLog = NULL;

	glGetShaderiv(ASM_vertexShaderObjectFB, GL_COMPILE_STATUS, &ASM_iShaderCompliedStatus);

	if (ASM_iShaderCompliedStatus == GL_FALSE)
	{
		glGetShaderiv(ASM_vertexShaderObjectFB, GL_INFO_LOG_LENGTH, &ASM_iInfoLogLength);

		if (ASM_iInfoLogLength > 0)
		{
			ASM_szInfoLog = (char*)malloc(sizeof(char) * ASM_iInfoLogLength);
			if (ASM_szInfoLog != NULL)
			{
				GLsizei ASM_written;
				glGetShaderInfoLog(ASM_vertexShaderObjectFB, ASM_iInfoLogLength, &ASM_written, ASM_szInfoLog);
				fprintf(ASM_gpFile, "Vertex Shader Compilation Log : %s\n", ASM_szInfoLog);
				free(ASM_szInfoLog);
				DestroyWindow(ASM_ghwnd);
			}

		}

	}
	////-----------------------------Fragment Shader

	ASM_fragmentShaderObjectFB = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* ASM_fragmentShaderSourceCodeFB =
		"#version 450 core" \
		"\n" \
		"in vec4 ASM_out_Color;" \
		"out vec4 ASM_fragColor;" \
		"void main(void)" \
		"{" \
		"ASM_fragColor = ASM_out_Color;" \
		"}";


	glShaderSource(ASM_fragmentShaderObjectFB, 1, (const GLchar**)&ASM_fragmentShaderSourceCodeFB, NULL);

	glCompileShader(ASM_fragmentShaderObjectFB);

	ASM_iInfoLogLength = 0;
	ASM_iShaderCompliedStatus = 0;
	ASM_szInfoLog = NULL;

	glGetShaderiv(ASM_fragmentShaderObjectFB, GL_COMPILE_STATUS, &ASM_iShaderCompliedStatus);

	if (ASM_iShaderCompliedStatus == GL_FALSE)
	{
		glGetShaderiv(ASM_fragmentShaderObjectFB, GL_INFO_LOG_LENGTH, &ASM_iInfoLogLength);

		if (ASM_iInfoLogLength > 0)
		{
			ASM_szInfoLog = (char*)malloc(sizeof(char) * ASM_iInfoLogLength);
			if (ASM_szInfoLog != NULL)
			{
				GLsizei ASM_written;
				glGetShaderInfoLog(ASM_fragmentShaderObjectFB, ASM_iInfoLogLength, &ASM_written, ASM_szInfoLog);
				fprintf(ASM_gpFile, "Fragment Shader Compilation Log : %s\n", ASM_szInfoLog);
				free(ASM_szInfoLog);
				DestroyWindow(ASM_ghwnd);
			}

		}

	}

	// -------------------------- Shader Program

	ASM_shaderProgramObjectFB = glCreateProgram();

	glAttachShader(ASM_shaderProgramObjectFB, ASM_vertexShaderObjectFB);

	glAttachShader(ASM_shaderProgramObjectFB, ASM_fragmentShaderObjectFB);

	glBindAttribLocation(ASM_shaderProgramObjectFB, ASM_ATTRIBUTE_POSITION, "ASM_vPosition");
	glBindAttribLocation(ASM_shaderProgramObjectFB, ASM_ATTRIBUTE_COLOR, "ASM_vColor");


	glLinkProgram(ASM_shaderProgramObjectFB);

	ASM_iInfoLogLength = 0;
	ASM_szInfoLog = NULL;
	GLint ASM_iShaderProgramLinkStatus = 0;

	glGetShaderiv(ASM_shaderProgramObjectFB, GL_LINK_STATUS, &ASM_iShaderProgramLinkStatus);

	if (ASM_iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetShaderiv(ASM_shaderProgramObjectFB, GL_INFO_LOG_LENGTH, &ASM_iInfoLogLength);

		if (ASM_iInfoLogLength > 0)
		{
			ASM_szInfoLog = (char*)malloc(sizeof(char) * ASM_iInfoLogLength);
			if (ASM_szInfoLog != NULL)
			{
				GLsizei ASM_written;
				glGetShaderInfoLog(ASM_shaderProgramObjectFB, ASM_iInfoLogLength, &ASM_written, ASM_szInfoLog);
				fprintf(ASM_gpFile, "Shader Program Link Log : %s\n", ASM_szInfoLog);
				free(ASM_szInfoLog);
				DestroyWindow(ASM_ghwnd);
			}

		}

	}

	ASM_mvpUniformFB = glGetUniformLocation(ASM_shaderProgramObjectFB, "ASM_u_mvpMatrix");

	const GLfloat ASM_cubeVerticesFB[] = {
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f
	};

	const GLfloat ASM_cubeColorFB[] = {
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};


	

	glGenVertexArrays(1, &ASM_vao_CubeFB);
		glBindVertexArray(ASM_vao_CubeFB);

		glGenBuffers(1, &ASM_vbo_CubePositionFB);
		glBindBuffer(GL_ARRAY_BUFFER, ASM_vbo_CubePositionFB);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_cubeVerticesFB), ASM_cubeVerticesFB, GL_STATIC_DRAW);

		glVertexAttribPointer(ASM_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(ASM_ATTRIBUTE_POSITION);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &ASM_vbo_CubeColorFB);
		glBindBuffer(GL_ARRAY_BUFFER, ASM_vbo_CubeColorFB);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_cubeColorFB), ASM_cubeColorFB, GL_STATIC_DRAW);

		glVertexAttribPointer(ASM_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(ASM_ATTRIBUTE_COLOR);

		glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);

	/////////----------------------------------------------------------------------------------------------- Default Scene
	////-----------------------------Vertex Shader

	//Create Shader
	ASM_vertexShaderObjectDefault = glCreateShader(GL_VERTEX_SHADER);

	//provide source code

	const GLchar *ASM_vertexShaderSourceCodeDefault =
		"#version 450 core" \
		"\n" \
		"in vec4 ASM_vPosition;" \
		"in vec2 ASM_vTexCoord;" \
		"uniform mat4 ASM_u_mvpMatrix;" \
		"out vec2 ASM_out_TexCoord;" \
		"void main(void)" \
		"{" \
			"gl_Position = ASM_u_mvpMatrix * ASM_vPosition;" \
			"ASM_out_TexCoord = ASM_vTexCoord;" \
		"}";

	glShaderSource(ASM_vertexShaderObjectDefault, 1, (const GLchar **)&ASM_vertexShaderSourceCodeDefault, NULL);

		//compile Shader
	glCompileShader(ASM_vertexShaderObjectDefault);

	ASM_iInfoLogLength = 0;
	ASM_iShaderCompliedStatus = 0;
	ASM_szInfoLog = NULL;

	glGetShaderiv(ASM_vertexShaderObjectDefault, GL_COMPILE_STATUS, &ASM_iShaderCompliedStatus);
	
	if (ASM_iShaderCompliedStatus == GL_FALSE)
	{
		glGetShaderiv(ASM_vertexShaderObjectDefault, GL_INFO_LOG_LENGTH, &ASM_iInfoLogLength);

		if (ASM_iInfoLogLength > 0)
		{
			ASM_szInfoLog = (char*)malloc(sizeof(char)* ASM_iInfoLogLength);
			if (ASM_szInfoLog != NULL)
			{
				GLsizei ASM_written;
				glGetShaderInfoLog(ASM_vertexShaderObjectDefault, ASM_iInfoLogLength, &ASM_written, ASM_szInfoLog);
				fprintf(ASM_gpFile, "Vertex Shader Compilation Log : %s\n", ASM_szInfoLog);
				free(ASM_szInfoLog);
				DestroyWindow(ASM_ghwnd);
			}

		}

	}
		////-----------------------------Fragment Shader

	ASM_fragmentShaderObjectDefault = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar *ASM_fragmentShaderSourceCode = 
		"#version 450 core" \
		"\n" \
		"in vec2 ASM_out_TexCoord;" \
		"uniform sampler2D ASM_u_Texture_Sampler;" \
		"out vec4 ASM_fragColor;" \
		"void main(void)" \
		"{" \
			"ASM_fragColor = texture(ASM_u_Texture_Sampler, ASM_out_TexCoord);" \
		"}";


	glShaderSource(ASM_fragmentShaderObjectDefault, 1, (const GLchar **)&ASM_fragmentShaderSourceCode,NULL);

	glCompileShader(ASM_fragmentShaderObjectDefault);

	ASM_iInfoLogLength = 0;
	ASM_iShaderCompliedStatus = 0;
	ASM_szInfoLog = NULL;

	glGetShaderiv(ASM_fragmentShaderObjectDefault, GL_COMPILE_STATUS, &ASM_iShaderCompliedStatus);

	if (ASM_iShaderCompliedStatus == GL_FALSE)
	{
		glGetShaderiv(ASM_fragmentShaderObjectDefault, GL_INFO_LOG_LENGTH, &ASM_iInfoLogLength);

		if (ASM_iInfoLogLength > 0)
		{
			ASM_szInfoLog = (char*)malloc(sizeof(char) * ASM_iInfoLogLength);
			if (ASM_szInfoLog != NULL)
			{
				GLsizei ASM_written;
				glGetShaderInfoLog(ASM_fragmentShaderObjectDefault, ASM_iInfoLogLength, &ASM_written, ASM_szInfoLog);
				fprintf(ASM_gpFile, "Fragment Shader Compilation Log : %s\n", ASM_szInfoLog);
				free(ASM_szInfoLog);
				DestroyWindow(ASM_ghwnd);
			}

		}

	}

	// -------------------------- Shader Program

	ASM_shaderProgramObjectDefault = glCreateProgram();

	glAttachShader(ASM_shaderProgramObjectDefault, ASM_vertexShaderObjectDefault);

	glAttachShader(ASM_shaderProgramObjectDefault, ASM_fragmentShaderObjectDefault);

	glBindAttribLocation(ASM_shaderProgramObjectDefault, ASM_ATTRIBUTE_POSITION, "ASM_vPosition");
	glBindAttribLocation(ASM_shaderProgramObjectDefault, ASM_ATTRIBUTE_TEXTURECOORD, "ASM_vTexCoord");
	

	glLinkProgram(ASM_shaderProgramObjectDefault);

	ASM_iInfoLogLength = 0;
	ASM_szInfoLog = NULL;
	ASM_iShaderProgramLinkStatus = 0;

	glGetShaderiv(ASM_shaderProgramObjectDefault, GL_LINK_STATUS, &ASM_iShaderProgramLinkStatus);

	if (ASM_iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetShaderiv(ASM_shaderProgramObjectDefault, GL_INFO_LOG_LENGTH, &ASM_iInfoLogLength);

		if (ASM_iInfoLogLength > 0)
		{
			ASM_szInfoLog = (char*)malloc(sizeof(char) * ASM_iInfoLogLength);
			if (ASM_szInfoLog != NULL)
			{
				GLsizei ASM_written;
				glGetShaderInfoLog(ASM_shaderProgramObjectDefault, ASM_iInfoLogLength, &ASM_written, ASM_szInfoLog);
				fprintf(ASM_gpFile, "Shader Program Link Log : %s\n", ASM_szInfoLog);
				free(ASM_szInfoLog);
				DestroyWindow(ASM_ghwnd);
			}

		}

	}

	ASM_mvpUniformDefault = glGetUniformLocation(ASM_shaderProgramObjectDefault, "ASM_u_mvpMatrix");
	ASM_textureSamplerUniform = glGetUniformLocation(ASM_shaderProgramObjectDefault, "ASM_u_Texture_Sampler");
	
	const GLfloat ASM_cubeVerticesDefault[] = {
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,

		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f
	};

	const GLfloat ASM_cubeTexArrayDefault[] = {
		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
		0.0f,1.0f,

		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
		0.0f,1.0f,

		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
		0.0f,1.0f,

		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
		0.0f,1.0f,

		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
		0.0f,1.0f,

		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
		0.0f,1.0f
	};

	
	glGenVertexArrays(1, &ASM_vao_CubeDefault);
	glBindVertexArray(ASM_vao_CubeDefault);
		
		glGenBuffers(1, &ASM_vbo_CubePositionDefault);
		glBindBuffer(GL_ARRAY_BUFFER, ASM_vbo_CubePositionDefault);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_cubeVerticesDefault), ASM_cubeVerticesDefault, GL_STATIC_DRAW);

		glVertexAttribPointer(ASM_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(ASM_ATTRIBUTE_POSITION);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &ASM_vbo_CubeTextureDefault);
		glBindBuffer(GL_ARRAY_BUFFER, ASM_vbo_CubeTextureDefault);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_cubeTexArrayDefault), ASM_cubeTexArrayDefault, GL_STATIC_DRAW);

		glVertexAttribPointer(ASM_ATTRIBUTE_TEXTURECOORD, 2, GL_FLOAT, GL_FALSE, 0 , NULL);
		glEnableVertexAttribArray(ASM_ATTRIBUTE_TEXTURECOORD);

		glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);

	//Depth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_TEXTURE_2D);

	createFramebuffer();
	
	
	

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

void createFramebuffer()
{

	glGenFramebuffers(1, &ASM_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, ASM_frameBuffer);


	glGenTextures(1, &ASM_texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ASM_texColorBuffer);

	//Setting of Texture Parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ASM_FRAMEBUFFER_WIDTH, ASM_FRAMEBUFFER_HEIGHT, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, NULL);
		
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ASM_texColorBuffer, 0);


	glGenRenderbuffers(1, &ASM_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, ASM_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, ASM_FRAMEBUFFER_WIDTH, ASM_FRAMEBUFFER_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ASM_rbo);

	fprintf(ASM_gpFile, "In FrameBuffer\n");
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		fprintf(ASM_gpFile, "Failed to Create FrameBuffer\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Resize(int width, int height)
{
	//code
	if (height <= 0)
		height = 1;

	ASM_WIN_WIDTH = width;
	ASM_WIN_HEIGHT = height;
	
}

void Display()
{
	void Update();

	ASM_perspectiveProjectionMatrix = mat4::identity();

	glViewport(0, 0, (GLsizei)ASM_FRAMEBUFFER_WIDTH, (GLsizei)ASM_FRAMEBUFFER_HEIGHT);
	ASM_perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)ASM_FRAMEBUFFER_WIDTH / (GLfloat)ASM_FRAMEBUFFER_HEIGHT, 0.1f, 100.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, ASM_frameBuffer);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glUseProgram(ASM_shaderProgramObjectFB);

			mat4 ASM_translateMatrix = mat4::identity();
			mat4 ASM_scaleMatrix = mat4::identity();
			mat4 ASM_XrotationMatrix = mat4::identity();
			mat4 ASM_YrotationMatrix = mat4::identity();
			mat4 ASM_ZrotationMatrix = mat4::identity();
			mat4 ASM_modelViewMatrix = mat4::identity();
			mat4 ASM_modelViewProjectMatrix = mat4::identity();

			ASM_translateMatrix = translate(0.0f, 0.0f, -7.0f);
			ASM_scaleMatrix = scale(1.0f, 1.0f, 1.0f);
			ASM_XrotationMatrix = rotate(ASM_CubeAngle, 1.0f, 0.0f, 0.0f);
			ASM_YrotationMatrix = rotate(ASM_CubeAngle, 0.0f, 1.0f, 0.0f);
			ASM_ZrotationMatrix = rotate(ASM_CubeAngle, 0.0f, 0.0f, 1.0f);

			ASM_modelViewMatrix = ASM_translateMatrix * ASM_scaleMatrix * ASM_XrotationMatrix * ASM_YrotationMatrix * ASM_ZrotationMatrix;

			ASM_modelViewProjectMatrix = ASM_perspectiveProjectionMatrix * ASM_modelViewMatrix;

			glUniformMatrix4fv(ASM_mvpUniformFB, 1, GL_FALSE, ASM_modelViewProjectMatrix);

			glBindVertexArray(ASM_vao_CubeFB);

				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
				glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
				glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
				glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
				glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

			glBindVertexArray(0);

		glUseProgram(0);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
	//code

	ASM_perspectiveProjectionMatrix = mat4::identity();

	glViewport(0, 0, (GLsizei)ASM_WIN_WIDTH, (GLsizei)ASM_WIN_HEIGHT);
	ASM_perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)ASM_WIN_WIDTH / (GLfloat)ASM_WIN_HEIGHT, 0.1f, 100.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ASM_shaderProgramObjectDefault);

		ASM_translateMatrix = mat4::identity();
		ASM_XrotationMatrix = mat4::identity();
		ASM_YrotationMatrix = mat4::identity();
		ASM_ZrotationMatrix = mat4::identity();
		ASM_modelViewMatrix = mat4::identity();
		ASM_modelViewProjectMatrix = mat4::identity();

		ASM_translateMatrix = translate(0.0f, 0.0f, -5.0f);
		ASM_XrotationMatrix = rotate(ASM_CubeAngle, 1.0f, 0.0f, 0.0f);
		ASM_YrotationMatrix = rotate(ASM_CubeAngle, 0.0f, 1.0f, 0.0f);
		ASM_ZrotationMatrix = rotate(ASM_CubeAngle, 0.0f, 0.0f, 1.0f);

		ASM_modelViewMatrix = ASM_translateMatrix* ASM_XrotationMatrix * ASM_YrotationMatrix * ASM_ZrotationMatrix;

		ASM_modelViewProjectMatrix = ASM_perspectiveProjectionMatrix * ASM_modelViewMatrix;

		glUniformMatrix4fv(ASM_mvpUniformDefault, 1, GL_FALSE, ASM_modelViewProjectMatrix);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ASM_texColorBuffer);
		glUniform1i(ASM_textureSamplerUniform, 0);

		glBindVertexArray(ASM_vao_CubeDefault);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 20, 4);


		glBindVertexArray(0);

	glUseProgram(0);

	Update();
	
	SwapBuffers(ASM_ghdc);

}

void Update()
{
	ASM_CubeAngle = ASM_CubeAngle + 0.5f;

	if (ASM_CubeAngle >= 360.0f)
	{
		ASM_CubeAngle = 0.0f;
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

	if (ASM_vao_CubeDefault)
	{
		glDeleteVertexArrays(1, &ASM_vao_CubeDefault);
		ASM_vao_CubeDefault = 0;
	}

	if (ASM_vbo_CubePositionDefault)
	{
		glDeleteBuffers(1, &ASM_vbo_CubePositionDefault);
		ASM_vbo_CubePositionDefault = 0;
	}

	if (ASM_vbo_CubeTextureDefault)
	{
		glDeleteBuffers(1, &ASM_vbo_CubeTextureDefault);
		ASM_vbo_CubeTextureDefault = 0;
	}

	if (ASM_vao_CubeFB)
	{
		glDeleteVertexArrays(1, &ASM_vao_CubeFB);
		ASM_vao_CubeFB = 0;
	}

	if (ASM_vbo_CubePositionFB)
	{
		glDeleteBuffers(1, &ASM_vbo_CubePositionFB);
		ASM_vbo_CubePositionFB = 0;
	}

	if (ASM_vbo_CubeColorFB)
	{
		glDeleteBuffers(1, &ASM_vbo_CubeColorFB);
		ASM_vbo_CubeColorFB = 0;
	}


	////--------------------------------- Safe Shader Cleanup
	if(ASM_shaderProgramObjectDefault)
	{

		glUseProgram(ASM_shaderProgramObjectDefault);

		GLsizei ASM_shaderCount;

		glGetProgramiv(ASM_shaderProgramObjectDefault, GL_ATTACHED_SHADERS, &ASM_shaderCount);

		GLuint * ASM_pShaders = NULL;

		ASM_pShaders = (GLuint *)malloc(ASM_shaderCount*sizeof(GLuint));

		if(ASM_pShaders == NULL)
		{
			fprintf(ASM_gpFile, "Memory Allocation Failed !!!!!\n");
			exit(0);

		}

		glGetAttachedShaders(ASM_shaderProgramObjectDefault, ASM_shaderCount,&ASM_shaderCount, ASM_pShaders);

		for(GLsizei ASM_i = 0; ASM_i < ASM_shaderCount; ASM_i++)
		{

			glDetachShader(ASM_shaderProgramObjectDefault, ASM_pShaders[ASM_i]);
			glDeleteShader(ASM_pShaders[ASM_i]);
			ASM_pShaders[ASM_i] = 0;

		}

		free(ASM_pShaders);
		ASM_pShaders = NULL;

		glDeleteProgram(ASM_shaderProgramObjectDefault);
		ASM_shaderProgramObjectDefault = 0;

		glUseProgram(0);

	}

	////--------------------------------- Safe Shader Cleanup
	if (ASM_shaderProgramObjectFB)
	{

		glUseProgram(ASM_shaderProgramObjectFB);

		GLsizei ASM_shaderCount = 0;

		glGetProgramiv(ASM_shaderProgramObjectFB, GL_ATTACHED_SHADERS, &ASM_shaderCount);

		GLuint * ASM_pShaders = NULL;

		ASM_pShaders = (GLuint*)malloc(ASM_shaderCount * sizeof(GLuint));

		if (ASM_pShaders == NULL)
		{
			fprintf(ASM_gpFile, "Memory Allocation Failed !!!!!\n");
			exit(0);

		}

		glGetAttachedShaders(ASM_shaderProgramObjectFB, ASM_shaderCount, &ASM_shaderCount, ASM_pShaders);

		for (GLsizei ASM_i = 0; ASM_i < ASM_shaderCount; ASM_i++)
		{

			glDetachShader(ASM_shaderProgramObjectFB, ASM_pShaders[ASM_i]);
			glDeleteShader(ASM_pShaders[ASM_i]);
			ASM_pShaders[ASM_i] = 0;

		}

		free(ASM_pShaders);
		ASM_pShaders = NULL;

		glDeleteProgram(ASM_shaderProgramObjectFB);
		ASM_shaderProgramObjectFB = 0;

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
