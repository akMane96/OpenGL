//header files
#include <windows.h>
#include<stdio.h>

#include<gl/glew.h>
#include <gl/gl.h>
#include<CL/opencl.h>

#include "icon.h"
#include "vmath.h"

//Macros
#define  ASM_WIN_WIDTH 800
#define ASM_WIN_HEIGHT 600
#define ASM_MESH_WIDTH 1024
#define ASM_MESH_HEIGHT 1024
#define ASM_MYARRAY_SIZE (ASM_MESH_WIDTH*ASM_MESH_HEIGHT*4)

using namespace vmath;

//Pragma
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "opencl.lib")

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

GLuint ASM_vao;
GLuint ASM_vbo;
GLuint ASM_vboGPU;
GLuint ASM_mvpUniform;

mat4 ASM_perspectiveProjectionMatrix;
GLfloat ASM_position[ASM_MESH_WIDTH][ASM_MESH_HEIGHT][4];
GLfloat ASM_animationTime = 0.0f;
bool ASM_beOnGPU = false;

cl_int ASM_clResult;
cl_mem cl_graphics_resource;
cl_device_id ASM_oclComputeDeviceID;
cl_context ASM_oclContext;
cl_command_queue ASM_oclCommandQueue;
cl_program ASM_oclProgram;
cl_kernel ASM_oclKernel;
char *ASM_oclSourceCode = NULL;
size_t ASM_sizeKernelCodeLength;


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
		TEXT("Perspective Triangle"),
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

			switch(wParam)
			{
				case 'C':
				case 'c':
					ASM_beOnGPU = false;
					break;

				case 'G':
				case 'g':
					ASM_beOnGPU = true;
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
	char* loadOCLProgramSourceCode(const char *, const char *, size_t *);

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

	cl_platform_id ASM_localPlatformID;
	cl_device_id *ASM_localOCLDeviceIds;
	cl_uint ASM_devCount;

	ASM_clResult = clGetPlatformIDs(1, &ASM_localPlatformID, NULL);
	if(ASM_clResult != CL_SUCCESS)
	{
		fprintf(ASM_gpFile, "clGetPlatformIDs() Failed !!!!!\n");
		DestroyWindow(ASM_ghwnd);
	}

	ASM_clResult = clGetDeviceIDs(ASM_localPlatformID, CL_DEVICE_TYPE_GPU, 0, NULL, &ASM_devCount);
	if(ASM_clResult != CL_SUCCESS)
	{
		fprintf(ASM_gpFile, "clGetDeviceIDs() Failed !!!!!\n");
		DestroyWindow(ASM_ghwnd);
	}else if (ASM_devCount == 0)
	{
		fprintf(ASM_gpFile, "clGetDeviceIDs() Failed !!!!!\n");
		DestroyWindow(ASM_ghwnd);
	} else 
	{
		ASM_localOCLDeviceIds = (cl_device_id *)malloc(sizeof(cl_device_id) * ASM_devCount);
		ASM_clResult = clGetDeviceIDs(ASM_localPlatformID, CL_DEVICE_TYPE_GPU, ASM_devCount, ASM_localOCLDeviceIds, NULL);
		if(ASM_clResult != CL_SUCCESS)
		{
			fprintf(ASM_gpFile, "clGetDeviceIDs() Failed !!!!!\n");
			DestroyWindow(ASM_ghwnd);
		}

		ASM_oclComputeDeviceID = ASM_localOCLDeviceIds[0];

		free(ASM_localOCLDeviceIds);
	}


	cl_context_properties ASM_context_properties[] = {
		CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
		CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
		CL_CONTEXT_PLATFORM, (cl_context_properties)ASM_localPlatformID,
		0
	};

	ASM_oclContext = clCreateContext(ASM_context_properties, 1, &ASM_oclComputeDeviceID, NULL, NULL, &ASM_clResult);
	if(ASM_clResult != CL_SUCCESS)
	{
		fprintf(ASM_gpFile, "clCreateContext() Failed !!!!!\n");
		DestroyWindow(ASM_ghwnd);
	}

	ASM_oclCommandQueue = clCreateCommandQueueWithProperties(ASM_oclContext, ASM_oclComputeDeviceID, 0, &ASM_clResult);
	if(ASM_clResult != CL_SUCCESS)
	{
		fprintf(ASM_gpFile, "clCreateCommandQueueWithProperties() Failed !!!!!\n");
		DestroyWindow(ASM_ghwnd);
	}

	ASM_oclSourceCode = loadOCLProgramSourceCode("SineWave.cl","",&ASM_sizeKernelCodeLength);

	//cl_int status = 0;
	ASM_oclProgram = clCreateProgramWithSource(ASM_oclContext, 1, (const char**)&ASM_oclSourceCode, &ASM_sizeKernelCodeLength, &ASM_clResult);
	if(ASM_clResult != CL_SUCCESS)
	{
		fprintf(ASM_gpFile, "clCreateProgramWithSource() Failed !!!!!\n");
		DestroyWindow(ASM_ghwnd);
	}

	ASM_clResult = clBuildProgram(ASM_oclProgram, 0, NULL, "-cl-fast-relaxed-math", NULL, NULL);
	if(ASM_clResult != CL_SUCCESS)
	{
		fprintf(ASM_gpFile, "clBuildProgram() Failed !!!!!\n");
		size_t ASM_len;
		char buffer[2048];
		clGetProgramBuildInfo(ASM_oclProgram, ASM_oclComputeDeviceID, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &ASM_len);
		fprintf(ASM_gpFile, "OpenCL Program Build Log : %s \n", buffer);

		DestroyWindow(ASM_ghwnd);
	}

	ASM_oclKernel = clCreateKernel(ASM_oclProgram, "SineWave_kernel", &ASM_clResult);
	if(ASM_clResult != CL_SUCCESS)
	{
		fprintf(ASM_gpFile, "clcreateKernel() Failed !!!!!\n");
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
		"uniform mat4 ASM_u_mvpMatrix;" \
		"void main(void)" \
		"{" \
		"gl_Position = ASM_u_mvpMatrix * ASM_vPosition;" \
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
		"out vec4 ASM_fragColor;" \
		"void main(void)" \
		"{" \
		"ASM_fragColor = vec4(1.0f, 0.5f, 0.0f, 1.0f);" \
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

	for(int i = 0; i < ASM_MESH_WIDTH; i++)
	{
		for(int j = 0; j < ASM_MESH_HEIGHT; j++){
			for(int k = 0; k < 4; k++){
				ASM_position[i][j][k] = 0.0f;
			}

		}
		
	}

	glGenVertexArrays(1, &ASM_vao);
	glBindVertexArray(ASM_vao);

		glGenBuffers(1, &ASM_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, ASM_vbo);
		glBufferData(GL_ARRAY_BUFFER, ASM_MYARRAY_SIZE * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &ASM_vboGPU);
		glBindBuffer(GL_ARRAY_BUFFER, ASM_vboGPU);
		glBufferData(GL_ARRAY_BUFFER, ASM_MYARRAY_SIZE * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		cl_graphics_resource = clCreateFromGLBuffer(ASM_oclContext, CL_MEM_WRITE_ONLY, ASM_vboGPU, &ASM_clResult);
		if(ASM_clResult != CL_SUCCESS)
		{
			fprintf(ASM_gpFile, "clCreateFromGLBuffer() Failed !!!!!\n");
			DestroyWindow(ASM_ghwnd);
		}

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

void launchCPUKernelAK(unsigned int meshWidth, unsigned int meshHeight, GLfloat animationTime)
{
	
	for(int i = 0; i < ASM_MESH_WIDTH; i++)
	{
		for(int j = 0; j < ASM_MESH_HEIGHT; j++){

			for(int k = 0; k < 4; k++)
			{

				GLfloat u = i/(GLfloat)meshWidth;
				GLfloat v = j/(GLfloat)meshHeight;
				u = u*2.0f - 1.0f;
				v = v*2.0f - 1.0f;
				GLfloat frequency = 4.0f;

				GLfloat w = sin(u*frequency + animationTime)*cos(v*frequency + animationTime)*0.5f;
				if(k == 0)
				{
					ASM_position[i][j][k] = u;
				} else if(k == 1)
				{
					ASM_position[i][j][k] = w;
				}
				else if(k == 2)
				{
					ASM_position[i][j][k] = v;
				}else if(k == 3)
				{
					ASM_position[i][j][k] = 1.0f;
				}
				
			}

		}
		
	}
	
}

void Display()
{

	size_t ASM_globalWorkSize[2];
	unsigned int meshWidth = ASM_MESH_WIDTH;
	unsigned int meshHeight = ASM_MESH_HEIGHT;
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ASM_shaderProgramObject);

		mat4 ASM_modelViewMatrix = mat4::identity();
		mat4 ASM_modelViewProjectMatrix = mat4::identity();
		mat4 ASM_translateMatrix = mat4::identity();				

		ASM_modelViewProjectMatrix = ASM_perspectiveProjectionMatrix * ASM_modelViewMatrix;

		glUniformMatrix4fv(ASM_mvpUniform, 1, GL_FALSE, ASM_modelViewProjectMatrix);
	
		glBindVertexArray(ASM_vao);

			if(ASM_beOnGPU == true)
			{
				//cl_graphics_resource = clCreateFromGLBuffer(ASM_oclContext, CL_MEM_WRITE_ONLY, ASM_vboGPU, &ASM_clResult);
				ASM_clResult = clSetKernelArg(ASM_oclKernel,0,sizeof(cl_mem), (void *)&cl_graphics_resource);
				if(ASM_clResult != CL_SUCCESS)
				{
					fprintf(ASM_gpFile, "clSetKernelArg() 1 Failed !!!!!\n");
					DestroyWindow(ASM_ghwnd);
				}

				ASM_clResult = clSetKernelArg(ASM_oclKernel,1,sizeof(cl_uint), (void *)&meshWidth);
				if(ASM_clResult != CL_SUCCESS)
				{
					fprintf(ASM_gpFile, "clSetKernelArg() 2 Failed !!!!!\n");
					DestroyWindow(ASM_ghwnd);
				}

				ASM_clResult = clSetKernelArg(ASM_oclKernel,2,sizeof(cl_uint), (void *)&meshHeight);
				if(ASM_clResult != CL_SUCCESS)
				{
					fprintf(ASM_gpFile, "clSetKernelArg() 3 Failed !!!!!\n");
					DestroyWindow(ASM_ghwnd);
				}

				ASM_clResult = clSetKernelArg(ASM_oclKernel,3,sizeof(cl_float), (void *)&ASM_animationTime);
				if(ASM_clResult != CL_SUCCESS)
				{
					fprintf(ASM_gpFile, "clSetKernelArg() 4 Failed !!!!!\n");
					DestroyWindow(ASM_ghwnd);
				}

				ASM_clResult = clEnqueueAcquireGLObjects(ASM_oclCommandQueue,1, &cl_graphics_resource,0, NULL, NULL );
				if(ASM_clResult != CL_SUCCESS)
				{
					fprintf(ASM_gpFile, "clEnqueueAcquireGLObjects() Failed !!!!!\n");
					DestroyWindow(ASM_ghwnd);
				}

				ASM_globalWorkSize[0] = ASM_MESH_WIDTH;
				ASM_globalWorkSize[1] = ASM_MESH_HEIGHT;

				ASM_clResult = clEnqueueNDRangeKernel(ASM_oclCommandQueue, ASM_oclKernel,2, NULL, ASM_globalWorkSize, 0,0, NULL,NULL );
				if(ASM_clResult != CL_SUCCESS)
				{
					fprintf(ASM_gpFile, "clEnqueueNDRangeKernel() Failed !!!!!\n");
					DestroyWindow(ASM_ghwnd);
				}

				ASM_clResult = clEnqueueReleaseGLObjects(ASM_oclCommandQueue,1, &cl_graphics_resource,0, NULL, NULL );
				if(ASM_clResult != CL_SUCCESS)
				{
					fprintf(ASM_gpFile, "clEnqueueAcquireGLObjects() Failed !!!!!\n");
					DestroyWindow(ASM_ghwnd);
				}

				clFinish(ASM_oclCommandQueue);

				glBindBuffer(GL_ARRAY_BUFFER, ASM_vboGPU);					


			}else
			{
				launchCPUKernelAK(ASM_MESH_WIDTH, ASM_MESH_HEIGHT, ASM_animationTime);
				glBindBuffer(GL_ARRAY_BUFFER, ASM_vbo);
					glBufferData(GL_ARRAY_BUFFER, ASM_MYARRAY_SIZE * sizeof(GLfloat), ASM_position, GL_DYNAMIC_DRAW);			
				 	
			}

			glVertexAttribPointer(ASM_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(ASM_ATTRIBUTE_POSITION);
			glDrawArrays(GL_POINTS, 0, ASM_MESH_WIDTH*ASM_MESH_HEIGHT); 
			glBindBuffer(GL_ARRAY_BUFFER, 0);			

		glBindVertexArray(0);


	glUseProgram(0);

	ASM_animationTime = ASM_animationTime + 0.01f;

	
	SwapBuffers(ASM_ghdc);

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

	if (ASM_vao)
	{
		glDeleteVertexArrays(1, &ASM_vao);
		ASM_vao = 0;
	}

	if (ASM_vbo)
	{
		glDeleteBuffers(1, &ASM_vbo);
		ASM_vbo = 0;
	}

	if (ASM_vboGPU)
	{
		glDeleteBuffers(1, &ASM_vboGPU);
		ASM_vboGPU = 0;
	}

	if(ASM_oclSourceCode)
	{
		free((void *)ASM_oclSourceCode);
		ASM_oclSourceCode= NULL;
	}

	if(cl_graphics_resource)
	{
		clReleaseMemObject(cl_graphics_resource);
		cl_graphics_resource = NULL;
	}

	if(ASM_oclKernel)
	{
		clReleaseKernel(ASM_oclKernel);
		ASM_oclKernel = NULL;

	}

	if(ASM_oclProgram)
	{
		clReleaseProgram(ASM_oclProgram);
		ASM_oclProgram = NULL;

	}

	if(ASM_oclCommandQueue)
	{
		clReleaseCommandQueue(ASM_oclCommandQueue);
		ASM_oclCommandQueue = NULL;

	}


	if(ASM_oclContext)
	{
		clReleaseContext(ASM_oclContext);
		ASM_oclContext = NULL;

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


char* loadOCLProgramSourceCode(const char *filename, const char *preamble, size_t *sizeFinalLength)
{
	FILE *pFile = NULL;
	size_t sizeSourceLength;

	pFile = fopen(filename, "rb");
	if(pFile == NULL)
		return(NULL);

	size_t sizePerambleLength = (size_t)strlen(preamble);

	fseek(pFile,0,SEEK_END);
	sizeSourceLength = ftell(pFile);
	fseek(pFile,0,SEEK_SET);

	char *sourceString=(char*)malloc(sizeSourceLength+sizePerambleLength+1);
	memcpy(sourceString, preamble, sizePerambleLength);

	if(fread((sourceString) + sizePerambleLength, sizeSourceLength,1,pFile)!=1)
	{
		fclose(pFile);
		free(sourceString);
		return(0);
	}

	fclose(pFile);

	if(sizeFinalLength != 0)
	{
		*sizeFinalLength = sizeSourceLength + sizePerambleLength;
	}

	sourceString[sizeSourceLength + sizePerambleLength] = '\0';

	return(sourceString);

}
