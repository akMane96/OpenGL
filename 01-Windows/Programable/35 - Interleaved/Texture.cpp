//header files
#include <windows.h>
#include<stdio.h>

#include<gl/glew.h>
#include <gl/gl.h>

#include "ogl.h"
#include "vmath.h"

//Macros
#define  ASM_WIN_WIDTH 800
#define ASM_WIN_HEIGHT 600

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

/////-------------------------------------------- Vertex and Shader Objecy
GLuint ASM_vertexShaderObject;
GLuint ASM_fragmentShaderObject;
GLuint ASM_shaderProgramObject;

/////-------------------------------------------- VAO and VBO
GLuint ASM_vao_Cube;
GLuint ASM_vbo_Cube;
/////-------------------------------------------- MVP Uniform

GLuint ASM_modelUniform;
GLuint ASM_viewUniform;
GLuint ASM_projectionUniform;

/////-------------------------------------------- Light Uniform

GLuint ASM_lAUniform;
GLuint ASM_lDUniform;
GLuint ASM_lSUniform;
GLuint ASM_lightPositionUniform;

/////-------------------------------------------- Light Arrays

vec3 ASM_lAVector = { 0.1f, 0.1f, 0.1f };
vec3 ASM_lDVector = { 1.0f, 1.0f , 1.0f };
vec3 ASM_lSVector = { 1.0f, 1.0f , 1.0f };
vec4 ASM_lightPositionVector = { 0.0f, 0.0f, 10.0f, 1.0f };

/////-------------------------------------------- Material Uniform

GLuint ASM_kAUniform;
GLuint ASM_kDUniform;
GLuint ASM_kSUniform;
GLuint ASM_materialShininessUniform;

/////-------------------------------------------- Material Arrays

vec3 ASM_kAVector = { 0.0f, 0.0f, 0.0f };
vec3 ASM_kDVector = { 1.0f, 1.0f , 1.0f };
vec3 ASM_kSVector = { 1.0f, 1.0f , 1.0f};
GLfloat ASM_materialShininess = 128.0f;

/////-------------------------------------------- Other
GLuint ASM_lKeyPressedUniform;
bool ASM_bLight;


GLuint ASM_textureSamplerUniform;
mat4 ASM_perspectiveProjectionMatrix;
GLfloat ASM_CubeAngle;
GLuint ASM_stoneTexture;

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

		case WM_CHAR:
			switch (wParam)
			{

			case 'l':
			case 'L':
				if (ASM_bLight == false)
				{
					ASM_bLight = true;
				}
				else
				{
					ASM_bLight = false;
				}

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
	bool loadGLTexture(GLuint*, TCHAR[]);

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

	// const GLchar *ASM_vertexShaderSourceCode = 
	// 	"#version 450 core" \
	// 	"\n" \
	// 	"in vec4 ASM_vPosition;" \
		
	// 	"uniform mat4 ASM_u_mvpMatrix;" \
	// 	"out vec2 ASM_out_TexCoord;" \
	// 	"out vec4 ASM_out_Color;" \
	// 	"void main(void)" \
	// 	"{" \
	// 		"gl_Position = ASM_u_mvpMatrix * ASM_vPosition;" \
	// 		"ASM_out_TexCoord = ASM_vTexCoord;" \
	// 		"ASM_out_Color = ASM_vColor;" \
	// 	"}";

	const GLchar* ASM_vertexShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"in vec4 ASM_vPosition;" \
		"in vec3 ASM_vNormal;" \
		"in vec2 ASM_vTexCoord;" \
		"in vec4 ASM_vColor;" \
		"uniform mat4 ASM_u_modelMatrix;" \
		"uniform mat4 ASM_u_viewpMatrix;" \
		"uniform mat4 ASM_u_projectionMatrix;" \
		"uniform vec4 ASM_u_lightPosition;" \
		"uniform int ASM_u_lKeyPressed;" \

		"out vec3 ASM_transformedNormal;" \
		"out vec3 ASM_lightDirection;" \
		"out vec3 ASM_viewerVector;" \
		"out vec2 ASM_out_TexCoord;" \
		"out vec4 ASM_out_Color;" \

		"void main(void)" \
		"{" \
			"if(ASM_u_lKeyPressed == 1)" \
			"{" \

				"vec4 ASM_eye_coordinates =  ASM_u_viewpMatrix * ASM_u_modelMatrix *  ASM_vPosition;" \
				"ASM_transformedNormal =  mat3(ASM_u_viewpMatrix * ASM_u_modelMatrix) * ASM_vNormal;" \
				"ASM_lightDirection =  vec3(ASM_u_lightPosition - ASM_eye_coordinates) ;" \
				"ASM_viewerVector = -(ASM_eye_coordinates.xyz);" \

			"}" \
						
			"gl_Position = ASM_u_projectionMatrix * ASM_u_viewpMatrix * ASM_u_modelMatrix * ASM_vPosition;" \
			"ASM_out_TexCoord = ASM_vTexCoord;" \
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

	// const GLchar *ASM_fragmentShaderSourceCode = 
	// 	"#version 450 core" \
	// 	"\n" \
	// 	"in vec2 ASM_out_TexCoord;" \
	// 	"in vec4 ASM_out_Color;" \
	// 	"uniform sampler2D ASM_u_Texture_Sampler;" \
	// 	"out vec4 ASM_fragColor;" \
	// 	"void main(void)" \
	// 	"{" \
	// 		"ASM_fragColor = texture(ASM_u_Texture_Sampler, ASM_out_TexCoord) * ASM_out_Color;" \
	// 	"}";

	const GLchar *ASM_fragmentShaderSourceCode = 
		"#version 450 core" \
		"\n" \
		"in vec3 ASM_transformedNormal;" \
		"in vec3 ASM_lightDirection;" \
		"in vec3 ASM_viewerVector;" \
		"in vec2 ASM_out_TexCoord;" \
		"in vec4 ASM_out_Color;" \
		
		"uniform vec3 ASM_u_lA;" \
		"uniform vec3 ASM_u_lD;" \
		"uniform vec3 ASM_u_lS;" \
		"uniform vec3 ASM_u_kA;" \
		"uniform vec3 ASM_u_kD;" \
		"uniform vec3 ASM_u_kS;" \
		"uniform float ASM_u_Shininess;" \
		"uniform int ASM_u_lKeyPressed;" \
		"uniform sampler2D ASM_u_Texture_Sampler;" \
		
		"out vec4 ASM_fragColor;" \

		"void main(void)" \
		"{" \

			"vec3  ASM_Phong_ADS_Light ;" \
			"if(ASM_u_lKeyPressed == 1)" \
			"{" \
				
				"vec3 ASM_normalizedTransformedNormal = normalize(ASM_transformedNormal) ;" \
				"vec3 ASM_normalizedLightDirection =  normalize(ASM_lightDirection) ;" \
				"vec3 ASM_normalizedViewerVector = normalize(ASM_viewerVector) ;" \

				"vec3 ASM_reflectionVector = reflect(-ASM_normalizedLightDirection, ASM_normalizedTransformedNormal);" \
				"vec3 ASM_ambient =  ASM_u_lA * ASM_u_kA;" \
				"vec3 ASM_diffuse =  ASM_u_lD * ASM_u_kD * max(dot(ASM_normalizedLightDirection, ASM_normalizedTransformedNormal), 0.0f);" \
				"vec3 ASM_specular =  ASM_u_lS * ASM_u_kS * pow(max(dot(ASM_reflectionVector, ASM_normalizedViewerVector), 0.0f), ASM_u_Shininess);" \
				"ASM_Phong_ADS_Light =  ASM_ambient + ASM_diffuse + ASM_specular;" \

			"}" \
			"else" \
			"{" \
				"ASM_Phong_ADS_Light = vec3(1.0f, 1.0f, 1.0f);" \
			"}" \

			"ASM_fragColor = vec4(ASM_Phong_ADS_Light, 1.0f) * texture(ASM_u_Texture_Sampler, ASM_out_TexCoord) * ASM_out_Color;" \
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
	glBindAttribLocation(ASM_shaderProgramObject, ASM_ATTRIBUTE_TEXTURECOORD, "ASM_vTexCoord");
	glBindAttribLocation(ASM_shaderProgramObject, ASM_ATTRIBUTE_COLOR, "ASM_vColor");
	glBindAttribLocation(ASM_shaderProgramObject, ASM_ATTRIBUTE_NORMAL, "ASM_vNormal");

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

	//// --------------------------------------------------------------------------------------- Matrix Uniform
	ASM_modelUniform = glGetUniformLocation(ASM_shaderProgramObject, "ASM_u_modelMatrix");
	ASM_viewUniform = glGetUniformLocation(ASM_shaderProgramObject, "ASM_u_viewpMatrix");
	ASM_projectionUniform = glGetUniformLocation(ASM_shaderProgramObject, "ASM_u_projectionMatrix");

	//// --------------------------------------------------------------------------------------- Light Uniform
	ASM_lAUniform = glGetUniformLocation(ASM_shaderProgramObject, "ASM_u_lA");
	ASM_lDUniform = glGetUniformLocation(ASM_shaderProgramObject, "ASM_u_lD");
	ASM_lSUniform = glGetUniformLocation(ASM_shaderProgramObject, "ASM_u_lS");
	ASM_lightPositionUniform = glGetUniformLocation(ASM_shaderProgramObject, "ASM_u_lightPosition");

	//// --------------------------------------------------------------------------------------- Material Uniform
	ASM_kAUniform = glGetUniformLocation(ASM_shaderProgramObject, "ASM_u_kA");
	ASM_kDUniform = glGetUniformLocation(ASM_shaderProgramObject, "ASM_u_kD");
	ASM_kSUniform = glGetUniformLocation(ASM_shaderProgramObject, "ASM_u_kS");
	ASM_materialShininessUniform = glGetUniformLocation(ASM_shaderProgramObject, "ASM_u_Shininess");

	//// --------------------------------------------------------------------------------------- Key Pressed Uniform
	ASM_lKeyPressedUniform = glGetUniformLocation(ASM_shaderProgramObject, "ASM_u_lKeyPressed");

	ASM_textureSamplerUniform = glGetUniformLocation(ASM_shaderProgramObject, "ASM_u_Texture_Sampler");
	
	const GLfloat ASM_cubeArray[] = {
		1.0f, 1.0f, 1.0f,1.0f, 0.0f, 0.0f,0.0f, 0.0f, 1.0f,0.0f,0.0f,
		-1.0f, 1.0f, 1.0f,1.0f, 0.0f, 0.0f,0.0f, 0.0f, 1.0f,1.0f,0.0f,
		-1.0f, -1.0f, 1.0f,1.0f, 0.0f, 0.0f,0.0f, 0.0f, 1.0f,1.0f,1.0f,
		1.0f, -1.0f, 1.0f,1.0f, 0.0f, 0.0f,0.0f, 0.0f, 1.0f,0.0f,1.0f,
		1.0f, 1.0f, -1.0f,0.0f, 1.0f, 0.0f,1.0f, 0.0f, 0.0f,0.0f,0.0f,
		1.0f, 1.0f, 1.0f,0.0f, 1.0f, 0.0f,1.0f, 0.0f, 0.0f,1.0f,0.0f,
		1.0f, -1.0f, 1.0f,0.0f, 1.0f, 0.0f,1.0f, 0.0f, 0.0f,1.0f,1.0f,
		1.0f, -1.0f, -1.0f,0.0f, 1.0f, 0.0f,1.0f, 0.0f, 0.0f,0.0f,1.0f,
		-1.0f, 1.0f, -1.0f,0.0f, 0.0f, 1.0f,0.0f, 0.0f, -1.0f,0.0f,0.0f,
		1.0f, 1.0f, -1.0f,0.0f, 0.0f, 1.0f,0.0f, 0.0f, -1.0f,1.0f,0.0f,
		1.0f, -1.0f, -1.0f,0.0f, 0.0f, 1.0f,0.0f, 0.0f, -1.0f,1.0f,1.0f,
		-1.0f, -1.0f, -1.0f,0.0f, 0.0f, 1.0f,0.0f, 0.0f, -1.0f,0.0f,1.0f,
		-1.0f, 1.0f, 1.0f,0.0f, 1.0f, 1.0f,-1.0f, 0.0f, 0.0f,0.0f,0.0f,
		-1.0f, 1.0f, -1.0f,0.0f, 1.0f, 1.0f,-1.0f, 0.0f, 0.0f,1.0f,0.0f,
		-1.0f, -1.0f, -1.0f,0.0f, 1.0f, 1.0f,-1.0f, 0.0f, 0.0f,1.0f,1.0f,
		-1.0f, -1.0f, 1.0f,0.0f, 1.0f, 1.0f,-1.0f, 0.0f, 0.0f,0.0f,1.0f,
		1.0f, 1.0f, -1.0f,1.0f, 0.0f, 1.0f,0.0f, 1.0f, 0.0f,0.0f,0.0f,
		-1.0f, 1.0f, -1.0f,1.0f, 0.0f, 1.0f,0.0f, 1.0f, 0.0f,1.0f,0.0f,
		-1.0f, 1.0f, 1.0f,1.0f, 0.0f, 1.0f,0.0f, 1.0f, 0.0f,1.0f,1.0f,
		1.0f, 1.0f, 1.0f,1.0f, 0.0f, 1.0f,0.0f, 1.0f, 0.0f,0.0f,1.0f,
		-1.0f, -1.0f, 1.0f,1.0f, 1.0f, 0.0f,0.0f, -1.0f, 0.0f,0.0f,0.0f,
		1.0f, -1.0f, 1.0f,1.0f, 1.0f, 0.0f,0.0f, -1.0f, 0.0f,1.0f,0.0f,
		1.0f, -1.0f, -1.0f,1.0f, 1.0f, 0.0f,0.0f, -1.0f, 0.0f,1.0f,1.0f,
		-1.0f, -1.0f, -1.0f,1.0f, 1.0f, 0.0f,0.0f, -1.0f, 0.0f,0.0f,1.0f

		};

	
	glGenVertexArrays(1, &ASM_vao_Cube);
	glBindVertexArray(ASM_vao_Cube);
		
		glGenBuffers(1, &ASM_vbo_Cube);
		glBindBuffer(GL_ARRAY_BUFFER, ASM_vbo_Cube);

			glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_cubeArray), ASM_cubeArray, GL_STATIC_DRAW);

			glVertexAttribPointer(ASM_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GL_FLOAT), (void*)0);
			glEnableVertexAttribArray(ASM_ATTRIBUTE_POSITION);

			glVertexAttribPointer(ASM_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GL_FLOAT) , (void*)(3*sizeof(GL_FLOAT)));
			glEnableVertexAttribArray(ASM_ATTRIBUTE_COLOR);

			glVertexAttribPointer(ASM_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GL_FLOAT) , (void*)(6*sizeof(GL_FLOAT)));
			glEnableVertexAttribArray(ASM_ATTRIBUTE_NORMAL);

			glVertexAttribPointer(ASM_ATTRIBUTE_TEXTURECOORD, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GL_FLOAT) , (void*)(9*sizeof(GL_FLOAT)));
			glEnableVertexAttribArray(ASM_ATTRIBUTE_TEXTURECOORD);					

		glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);

	//Depth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);	

	loadGLTexture(&ASM_stoneTexture, MAKEINTRESOURCE(ASM_STONE_BITMAP));
	
	glEnable(GL_TEXTURE_2D);


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

	ASM_perspectiveProjectionMatrix = mat4::identity();

	//Warm Up Resize
	Resize(ASM_WIN_WIDTH, ASM_WIN_HEIGHT);
	
}

bool loadGLTexture(GLuint* texture, TCHAR resourceId[])
{

	//varaible declaration
	bool ASM_bResult = false;
	HBITMAP ASM_hBitmap = NULL;
	BITMAP ASM_bmp;


	//code

	ASM_hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), resourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (ASM_hBitmap)
	{
		ASM_bResult = true;

		GetObject(ASM_hBitmap, sizeof(BITMAP), &ASM_bmp);

		//OpenGL Code
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);

		//Setting of Texture Parameter

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, ASM_bmp.bmWidth, ASM_bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, ASM_bmp.bmBits);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ASM_bmp.bmWidth, ASM_bmp.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, ASM_bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);

		DeleteObject(ASM_hBitmap);

	}


	return ASM_bResult;

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
	void Update();

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ASM_shaderProgramObject);

		mat4 ASM_modelMatrix = mat4::identity();
		mat4 ASM_viewMatrix = mat4::identity();
		mat4 ASM_translateMatrix = mat4::identity();
		mat4 ASM_scaleMatrix = mat4::identity();
		mat4 ASM_rotateMatrix = mat4::identity();

		ASM_translateMatrix = translate(0.0f, 0.0f, -7.0f);
		ASM_rotateMatrix = rotate(ASM_CubeAngle, 1.0f, 0.0f,0.0f);
		ASM_rotateMatrix = ASM_rotateMatrix * rotate(ASM_CubeAngle, 0.0f, 1.0f,0.0f);
		ASM_rotateMatrix = ASM_rotateMatrix * rotate(ASM_CubeAngle, 0.0f, 0.0f,1.0f);

		ASM_modelMatrix = ASM_translateMatrix * ASM_rotateMatrix;
		

		////-------------------------------------------------------------- Send Matrix Uniform

		glUniformMatrix4fv(ASM_modelUniform, 1, GL_FALSE, ASM_modelMatrix);
		glUniformMatrix4fv(ASM_viewUniform, 1, GL_FALSE, ASM_viewMatrix);
		glUniformMatrix4fv(ASM_projectionUniform, 1, GL_FALSE, ASM_perspectiveProjectionMatrix);


		if (ASM_bLight == true)
		{
			////-------------------------------------------------------------- Send Light Uniform

			glUniform3fv(ASM_lAUniform, 1, ASM_lAVector);
			glUniform3fv(ASM_lDUniform, 1, ASM_lDVector);
			glUniform3fv(ASM_lSUniform, 1, ASM_lSVector);
			glUniform4fv(ASM_lightPositionUniform, 1, ASM_lightPositionVector);

			////-------------------------------------------------------------- Send Material Uniform

			glUniform3fv(ASM_kAUniform, 1, ASM_kAVector);
			glUniform3fv(ASM_kDUniform, 1, ASM_kDVector);
			glUniform3fv(ASM_kSUniform, 1, ASM_kSVector);
			glUniform1f(ASM_materialShininessUniform, ASM_materialShininess);

			////-------------------------------------------------------------- Send Key Pressed Uniform
			glUniform1i(ASM_lKeyPressedUniform, 1);

		}
		else
		{
			glUniform1i(ASM_lKeyPressedUniform, 0);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ASM_stoneTexture);
		glUniform1i(ASM_textureSamplerUniform, 0);

		glBindVertexArray(ASM_vao_Cube);

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
	ASM_CubeAngle = ASM_CubeAngle + 0.05f;

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

	if (ASM_stoneTexture)
	{
		glDeleteTextures(1, &ASM_stoneTexture);
	}	

	if (ASM_vao_Cube)
	{
		glDeleteVertexArrays(1, &ASM_vao_Cube);
		ASM_vao_Cube = 0;
	}

	if (ASM_vbo_Cube)
	{
		glDeleteBuffers(1, &ASM_vbo_Cube);
		ASM_vbo_Cube = 0;
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
