//header files
#include <windows.h>
#include<stdio.h>

#include<gl/glew.h>
#include <gl/gl.h>

#include "icon.h"
#include "vmath.h"

//Macros
#define  ASM_WIN_WIDTH 800
#define ASM_WIN_HEIGHT 600

#define ASM_BUFFER_SIZE 256

#define ASM_NR_POINT_COORDS 3
#define ASM_NR_TEXTURE_COORDS 2
#define ASM_NR_NORMAL_COORDS 3
#define ASM_NR_FACE_TOKENS 3
#define ASM_NR_TRIANGLE_VERTICES 3 

using namespace vmath;

//Pragma
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

//Structure

// int Vector
typedef struct ASM_vec_2d_int
{
	GLint* ASM_pp_arr;
	size_t ASM_size;

} ASM_vec_2d_int_t;

// Float Vector
typedef struct ASM_vec_2d_float
{
	GLfloat* ASM_pp_arr;
	size_t ASM_size;

} ASM_vec_2d_float_t;

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
GLuint ASM_vbo_model_position;
GLuint ASM_vbo_model_normals;
GLuint ASM_vbo_model_element;
GLuint ASM_mvpUniform;

//Model Loading Varaiable
ASM_vec_2d_float_t* ASM_gp_vertices = NULL;
ASM_vec_2d_float_t* ASM_gp_texture = NULL;
ASM_vec_2d_float_t* ASM_gp_normals = NULL;

ASM_vec_2d_float_t* ASM_gp_vertices_sorted = NULL;
ASM_vec_2d_float_t* ASM_gp_texture_sorted = NULL;
ASM_vec_2d_float_t* ASM_gp_normals_sorted = NULL;

ASM_vec_2d_int_t* ASM_gp_face_tri = NULL, * ASM_gp_face_texture = NULL, * ASM_gp_face_normals = NULL;

FILE* ASM_g_fp_meshfile = NULL;

char ASM_g_line[ASM_BUFFER_SIZE];

mat4 ASM_perspectiveProjectionMatrix;

GLfloat ASM_angle;


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
	void LoadMeshData();

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
		"ASM_fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);" \
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

	LoadMeshData();

	for(int i = 0;i < ASM_gp_vertices->ASM_size; i++ )
	{
		fprintf(ASM_gpFile, "ASM_gp_vertices %f\n", ASM_gp_vertices->ASM_pp_arr[i]);
	}

	for(int i = 0;i < ASM_gp_face_tri->ASM_size; i++ )
	{
		fprintf(ASM_gpFile, "ASM_gp_face_tri %d\n", ASM_gp_face_tri->ASM_pp_arr[i]);
	}

	for(int i = 0;i < ASM_gp_face_tri->ASM_size; i++ )
	{
		fprintf(ASM_gpFile, "ASM_gp_face_tri %f\n", ASM_gp_vertices_sorted->ASM_pp_arr[i]);
	}



	
	glGenVertexArrays(1, &ASM_vao);
	glBindVertexArray(ASM_vao);

		glGenBuffers(1, &ASM_vbo_model_position);
			glBindBuffer(GL_ARRAY_BUFFER, ASM_vbo_model_position);
			glBufferData(GL_ARRAY_BUFFER, ASM_gp_vertices->ASM_size*sizeof(GLfloat), ASM_gp_vertices_sorted->ASM_pp_arr, GL_STATIC_DRAW);
	
			glVertexAttribPointer(ASM_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			glEnableVertexAttribArray(ASM_ATTRIBUTE_POSITION);

		glBindBuffer(GL_ARRAY_BUFFER, 0);	

		glGenBuffers(1, &ASM_vbo_model_element);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ASM_vbo_model_element);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, ASM_gp_face_tri->ASM_size * sizeof(int), ASM_gp_face_tri->ASM_pp_arr , GL_STATIC_DRAW);
	
		glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);

	//Depth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


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

void LoadMeshData()
{

	ASM_vec_2d_int_t* create_vec_2d_int(void);
	ASM_vec_2d_float_t* create_vec_2d_float(void);
	void push_back_vec_2d_int(ASM_vec_2d_int_t*, int);
	void push_back_vec_2d_float(ASM_vec_2d_float_t*, float);
	void* xcalloc(int, size_t);
	void displayFloat(ASM_vec_2d_float_t*);
	void displayInt(ASM_vec_2d_int_t*);

	ASM_g_fp_meshfile = fopen("MonkeyHead.obj", "r");

	if (!ASM_g_fp_meshfile)
	{
		fprintf(ASM_gpFile, "Unable to Open Model File \n");
		DestroyWindow(ASM_ghwnd);
	}

	ASM_gp_vertices = create_vec_2d_float();
	ASM_gp_texture = create_vec_2d_float();
	ASM_gp_normals = create_vec_2d_float();

	

	ASM_gp_face_tri = create_vec_2d_int();
	ASM_gp_face_texture = create_vec_2d_int();
	ASM_gp_face_normals = create_vec_2d_int();


	char* ASM_sep_space = " ";
	char* ASM_sep_fslash = "/";

	char* ASM_first_token = NULL;
	char* ASM_token = NULL;

	char* ASM_faceTokens[3];

	int ASM_nr_tokens;

	char* ASM_token_vertex_index = NULL;
	char* ASM_token_texture_index = NULL;
	char* ASM_token_normal_index = NULL;


	//code

	while (fgets(ASM_g_line, ASM_BUFFER_SIZE, ASM_g_fp_meshfile) != NULL)
	{
		ASM_first_token = strtok(ASM_g_line, ASM_sep_space);

		if (strcmp(ASM_first_token, "v") == 0)
		{
			while((ASM_token = strtok(NULL, ASM_sep_space)) != NULL)
				push_back_vec_2d_float(ASM_gp_vertices, atof(ASM_token));

		}
		else if (strcmp(ASM_first_token, "vt") == 0)
		{

			while ((ASM_token = strtok(NULL, ASM_sep_space)) != NULL)
				push_back_vec_2d_float(ASM_gp_texture, atof(ASM_token));			

		}
		else if (strcmp(ASM_first_token, "vn") == 0)
		{
			while ((ASM_token = strtok(NULL, ASM_sep_space)) != NULL)
				push_back_vec_2d_float(ASM_gp_normals, atof(ASM_token));		

		}
		else if (strcmp(ASM_first_token, "f") == 0)
		{
			for (int i = 0; i < ASM_NR_FACE_TOKENS; i++)
			{
				ASM_faceTokens[i] = strtok(NULL, ASM_sep_space);

			}


			for (int i = 0; i < ASM_NR_FACE_TOKENS; i++)
			{
				ASM_token_vertex_index = strtok(ASM_faceTokens[i], ASM_sep_fslash);
				ASM_token_texture_index = strtok(NULL, ASM_sep_fslash);
				ASM_token_normal_index = strtok(NULL, ASM_sep_fslash);	

				push_back_vec_2d_int(ASM_gp_face_tri, atoi(ASM_token_vertex_index) - 1);
				push_back_vec_2d_int(ASM_gp_face_texture, atoi(ASM_token_texture_index) - 1);
				push_back_vec_2d_int(ASM_gp_face_normals, atoi(ASM_token_normal_index) - 1);

			}		

		} 

		memset((void*)ASM_g_line, (int)'\0', ASM_BUFFER_SIZE);

	}


	ASM_gp_vertices_sorted = create_vec_2d_float();
	

	for (int i = 0; i < ASM_gp_face_tri->ASM_size; i++)
	{
		push_back_vec_2d_float(ASM_gp_vertices_sorted, ASM_gp_vertices->ASM_pp_arr[i]);
	}

	ASM_gp_normals_sorted = create_vec_2d_float();
	for (int i = 0; i < ASM_gp_face_normals->ASM_size; i++)
	{
		push_back_vec_2d_float(ASM_gp_normals_sorted, ASM_gp_normals->ASM_pp_arr[i]);
	}

	ASM_gp_texture_sorted = create_vec_2d_float();


	for (int i = 0; i < ASM_gp_face_texture->ASM_size; i++)
	{
		push_back_vec_2d_float(ASM_gp_texture_sorted, ASM_gp_texture->ASM_pp_arr[i]);
	}

	

	fclose(ASM_g_fp_meshfile);
	ASM_g_fp_meshfile = NULL;

	fprintf(ASM_gpFile, "gp_vertices size : %zu gp_texture size : %zu gp_normasl: %zu gp_face_tri :%zu gp_face_sorted :%zu \n", ASM_gp_vertices->ASM_size, ASM_gp_texture->ASM_size, ASM_gp_normals->ASM_size, ASM_gp_face_tri->ASM_size, ASM_gp_vertices_sorted->ASM_size);
	displayFloat(ASM_gp_vertices_sorted);
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
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ASM_shaderProgramObject);

		mat4 ASM_modelViewMatrix = mat4::identity();
		mat4 ASM_modelViewProjectMatrix = mat4::identity();
		mat4 ASM_translateMatrix = mat4::identity();
		mat4 ASM_rotateMatrix = mat4::identity();


		ASM_translateMatrix = translate(0.0f, 0.0f, -5.0f);
		ASM_rotateMatrix = rotate(ASM_angle,0.0f, 1.0f, 0.0f);

		ASM_modelViewMatrix = ASM_translateMatrix * ASM_rotateMatrix;

		ASM_modelViewProjectMatrix = ASM_perspectiveProjectionMatrix * ASM_modelViewMatrix;

		glUniformMatrix4fv(ASM_mvpUniform, 1, GL_FALSE, ASM_modelViewProjectMatrix);

		glBindVertexArray(ASM_vao);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ASM_vbo_model_element);
			glDrawElements(GL_TRIANGLES, ASM_gp_face_tri->ASM_size, GL_UNSIGNED_INT, NULL);

		glBindVertexArray(0);


	glUseProgram(0);

	ASM_angle = ASM_angle + 0.1f;
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

	if (ASM_vbo_model_position)
	{
		glDeleteBuffers(1, &ASM_vbo_model_position);
		ASM_vbo_model_position = 0;
	}

	if (ASM_vbo_model_normals)
	{
		glDeleteBuffers(1, &ASM_vbo_model_normals);
		ASM_vbo_model_normals = 0;
	}

	if (ASM_vbo_model_element)
	{
		glDeleteBuffers(1, &ASM_vbo_model_element);
		ASM_vbo_model_element = 0;
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


ASM_vec_2d_int_t* create_vec_2d_int(void)
{
	void* xcalloc(int, size_t);

	return (ASM_vec_2d_int_t*)xcalloc(1, sizeof(ASM_vec_2d_int_t));
}

ASM_vec_2d_float_t* create_vec_2d_float(void)
{
	void* xcalloc(int, size_t);

	return (ASM_vec_2d_float_t*)xcalloc(1, sizeof(ASM_vec_2d_float_t));

}

void push_back_vec_2d_int(ASM_vec_2d_int_t* p_vec, GLint data)
{
	void* xrealloc(void*, size_t);

	p_vec->ASM_pp_arr = (GLint*)xrealloc(p_vec->ASM_pp_arr, (p_vec->ASM_size + 1) * sizeof(GLint));
	p_vec->ASM_size++;
	p_vec->ASM_pp_arr[p_vec->ASM_size - 1] = data;

}

void push_back_vec_2d_float(ASM_vec_2d_float_t* p_vec, GLfloat data)
{
	void* xrealloc(void*, size_t);

	p_vec->ASM_pp_arr = (GLfloat*)xrealloc(p_vec->ASM_pp_arr, (p_vec->ASM_size + 1) * sizeof(GLfloat));
	p_vec->ASM_size++;
	p_vec->ASM_pp_arr[p_vec->ASM_size - 1] = data;

}

void clean_vec_2d_int(ASM_vec_2d_int_t* pp_vec)
{

	free(pp_vec->ASM_pp_arr);
	free(pp_vec);
}

void clean_vec_2d_float(ASM_vec_2d_float_t* pp_vec)
{

	free(pp_vec->ASM_pp_arr);
	free(pp_vec);

}

void* xcalloc(int nr_elements, size_t size_per_elements)
{
	void* p = calloc(nr_elements, size_per_elements);

	if (!p)
	{
		fprintf(ASM_gpFile, "Calloc:Failed to allocate memory.");
		DestroyWindow(ASM_ghwnd);

	}

	return(p);
}

void* xrealloc(void* p, size_t new_size)
{
	void* ptr = realloc(p, new_size);
	if (!ptr)
	{
		fprintf(ASM_gpFile, "Realloc:Failed to allocate memory.");
		DestroyWindow(ASM_ghwnd);

	}

	return(ptr);
}

void displayFloat(ASM_vec_2d_float_t* p_vec)
{

	for (int i = 0; i < p_vec->ASM_size; i++)
	{
		fprintf(ASM_gpFile, "%f ", p_vec->ASM_pp_arr[i]);
	}

}

void displayInt(ASM_vec_2d_int_t* p_vec)
{

	for (int i = 0; i < p_vec->ASM_size; i++)
	{
		fprintf(ASM_gpFile, "%d ", p_vec->ASM_pp_arr[i]);
	}

}
