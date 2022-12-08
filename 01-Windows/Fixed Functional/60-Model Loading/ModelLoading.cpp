//header files
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "icon.h"
#include<math.h>

//Macros
#define  ASM_WIN_WIDTH 800
#define ASM_WIN_HEIGHT 600

#define ASM_BUFFER_SIZE 256

#define ASM_NR_POINT_COORDS 3
#define ASM_NR_TEXTURE_COORDS 2
#define ASM_NR_NORMAL_COORDS 3
#define ASM_NR_FACE_TOKENS 3
#define ASM_NR_TRIANGLE_VERTICES 3 

//Pragma
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib,"glu32.lib")

//Structure

// int Vector
typedef struct ASM_vec_2d_int
{
	GLint** ASM_pp_arr;
	size_t ASM_size;

} ASM_vec_2d_int_t;

// Float Vector
typedef struct ASM_vec_2d_float
{
	GLfloat** ASM_pp_arr;
	size_t ASM_size;

} ASM_vec_2d_float_t;

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

//Model Loading Varaiable
ASM_vec_2d_float_t * ASM_gp_vertices = NULL;
ASM_vec_2d_float_t * ASM_gp_texture = NULL;
ASM_vec_2d_float_t* ASM_gp_normals = NULL;

ASM_vec_2d_int_t* ASM_gp_face_tri = NULL, * ASM_gp_face_texture = NULL, * ASM_gp_face_normals = NULL;

FILE* ASM_g_fp_meshfile = NULL;

char ASM_g_line[ASM_BUFFER_SIZE];



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
	ASM_wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC ;

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
		TEXT("Akshay Model Loading OpenGL"),
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
	void LoadMeshData();

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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//Depth
	glShadeModel(GL_SMOOTH);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	

	//Warm Up Resize
	Resize(ASM_WIN_WIDTH, ASM_WIN_HEIGHT);

	fprintf(ASM_gpFile, "Before LoadMesh\n");
	fflush(ASM_gpFile);

	LoadMeshData();

	fprintf(ASM_gpFile, "After LoadMesh\n");
	fflush(ASM_gpFile);
	
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

void LoadMeshData()
{

	ASM_vec_2d_int_t* create_vec_2d_int(void);
	ASM_vec_2d_float_t* create_vec_2d_float(void);
	void push_back_vec_2d_int(ASM_vec_2d_int_t *, int*);
	void push_back_vec_2d_float(ASM_vec_2d_float_t *, float*);
	void* xcalloc(int, size_t);

	ASM_g_fp_meshfile = fopen("MonkeyHead.obj", "r");

	if(!ASM_g_fp_meshfile)
		DestroyWindow(ASM_ghwnd);

	ASM_gp_vertices = create_vec_2d_float();
	ASM_gp_texture = create_vec_2d_float();
	ASM_gp_normals = create_vec_2d_float();

	ASM_gp_face_tri =  create_vec_2d_int();
	ASM_gp_face_texture = create_vec_2d_int();
	ASM_gp_face_normals = create_vec_2d_int();


	char* ASM_sep_space = " ";
	char* ASM_sep_fslash = "/";

	char* ASM_first_token = NULL;
	char* ASM_token = NULL;

	char* ASM_faceTokens[ASM_NR_FACE_TOKENS];

	int ASM_nr_tokens;

	char* ASM_token_vertex_index = NULL;
	char* ASM_token_texture_index = NULL;
	char* ASM_token_normal_index = NULL;


	//code

	while(fgets(ASM_g_line, ASM_BUFFER_SIZE, ASM_g_fp_meshfile) != NULL)
	{
		ASM_first_token = strtok(ASM_g_line, ASM_sep_space);

		if (strcmp(ASM_first_token, "v") == 0)
		{
			GLfloat* ASM_pvec_point_coord = (GLfloat*)xcalloc(ASM_NR_POINT_COORDS, sizeof(GLfloat));

			for (int i = 0; i != ASM_NR_POINT_COORDS; i++)
				ASM_pvec_point_coord[i] = atof(strtok(NULL, ASM_sep_space));

			push_back_vec_2d_float(ASM_gp_vertices , ASM_pvec_point_coord);

		}
		else if (strcmp(ASM_first_token, "vt") == 0)
		{

			GLfloat* ASM_pvec_texture_coord = (GLfloat*)xcalloc(ASM_NR_TEXTURE_COORDS, sizeof(GLfloat));

			for (int i = 0; i != ASM_NR_TEXTURE_COORDS; i++)
				ASM_pvec_texture_coord[i] = atof(strtok(NULL, ASM_sep_space));

				push_back_vec_2d_float(ASM_gp_texture , ASM_pvec_texture_coord);


		}
		else if (strcmp(ASM_first_token, "vn") == 0)
		{
			GLfloat* ASM_pvec_normal_coord = (GLfloat*)xcalloc(ASM_NR_NORMAL_COORDS, sizeof(GLfloat));

			for (int i = 0; i != ASM_NR_NORMAL_COORDS; i++)
				ASM_pvec_normal_coord[i] = atof(strtok(NULL, ASM_sep_space));

			push_back_vec_2d_float(ASM_gp_normals, ASM_pvec_normal_coord);

		}
		else if (strcmp(ASM_first_token, "f") == 0)
		{
			GLint* ASM_pvec_vertex_indices = (GLint*)xcalloc(3, sizeof(GLint));
			GLint* ASM_pvec_texture_indices = (GLint*)xcalloc(3, sizeof(GLint));
			GLint* ASM_pvec_normal_indices = (GLint*)xcalloc(3, sizeof(GLint));

			memset((void *)ASM_faceTokens, 0, ASM_NR_FACE_TOKENS);

			ASM_nr_tokens = 0;

			while (ASM_token = strtok(NULL, ASM_sep_space))
			{

				ASM_faceTokens[ASM_nr_tokens] = ASM_token;
				ASM_nr_tokens++;
			}


			for (int i = 0; i != ASM_NR_FACE_TOKENS; i++)
			{
				ASM_token_vertex_index = strtok(ASM_faceTokens[i], ASM_sep_fslash);
				ASM_token_texture_index = strtok(NULL, ASM_sep_fslash);
				ASM_token_normal_index = strtok(NULL, ASM_sep_fslash);

				ASM_pvec_vertex_indices[i] = atoi(ASM_token_vertex_index);
				ASM_pvec_texture_indices[i] = atoi(ASM_token_texture_index);
				ASM_pvec_normal_indices[i] = atoi(ASM_token_normal_index);


			}

			push_back_vec_2d_int(ASM_gp_face_tri, ASM_pvec_vertex_indices);
			push_back_vec_2d_int(ASM_gp_face_texture, ASM_pvec_texture_indices);
			push_back_vec_2d_int(ASM_gp_face_normals, ASM_pvec_normal_indices);

		}

		memset((void*)ASM_g_line, (int)'\0', ASM_BUFFER_SIZE);

	}

	fclose(ASM_g_fp_meshfile);
	ASM_g_fp_meshfile = NULL;

	fprintf(ASM_gpFile, "gp_vertices size : %zu gp_texture size : %zu gp_normasl: %zu gp_face_tri :%zu\n", ASM_gp_vertices->ASM_size, ASM_gp_texture->ASM_size, ASM_gp_normals->ASM_size, ASM_gp_face_tri->ASM_size);
	fflush(ASM_gpFile);

	fprintf(ASM_gpFile, "End of LoadMesh\n");
	fflush(ASM_gpFile);

	/*for (int i = 0; i != ASM_gp_vertices->ASM_size; i++)
	{
		fprintf(ASM_gpFile, "\n");
		fflush(ASM_gpFile);

		for (int j = 0; j != 3; j++)
		{
			fprintf(ASM_gpFile, "%f ", ASM_gp_vertices->ASM_pp_arr[i][j]);
			fflush(ASM_gpFile);		

		}
		
	}*/
	

}

void Display()
{
	void update();

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	
	glTranslatef(0.0f, 0.0f, -5.0f);
	glRotatef(ASM_angle, 0.0f, 1.0f,0.0f);
	glScalef(1.5f, 1.5f, 1.5f);

	glFrontFace(GL_CCW);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (int i = 0; i != ASM_gp_face_tri->ASM_size; i++)
	{

		glBegin(GL_TRIANGLES);
			for (int j = 0; j != ASM_NR_TRIANGLE_VERTICES; j++)
			{
				int vi = ASM_gp_face_tri->ASM_pp_arr[i][j] - 1;
				glVertex3f(ASM_gp_vertices->ASM_pp_arr[vi][0], ASM_gp_vertices->ASM_pp_arr[vi][1], ASM_gp_vertices->ASM_pp_arr[vi][2]);

			}

		glEnd();
	}

	update();
	SwapBuffers(ASM_ghdc);

}


void update()
{
	ASM_angle = ASM_angle + 0.5;


	if (ASM_angle > 360.0f)
		ASM_angle = 0.0f;
}

void UnInitialize()
{

	void clean_vec_2d_float(ASM_vec_2d_float_t**);
	void clean_vec_2d_int(ASM_vec_2d_int_t**);

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

	clean_vec_2d_float(&ASM_gp_vertices);
	clean_vec_2d_float(&ASM_gp_texture);
	clean_vec_2d_float(&ASM_gp_normals);

	clean_vec_2d_int(&ASM_gp_face_tri);
	clean_vec_2d_int(&ASM_gp_face_texture);
	clean_vec_2d_int(&ASM_gp_face_normals);

	
}


ASM_vec_2d_int_t* create_vec_2d_int(void)
{
	void* xcalloc(int, size_t);

	return (ASM_vec_2d_int_t*)xcalloc(1,sizeof(ASM_vec_2d_int_t));
}

ASM_vec_2d_float_t* create_vec_2d_float(void)
{
	void* xcalloc(int, size_t);

	return (ASM_vec_2d_float_t*)xcalloc(1, sizeof(ASM_vec_2d_float_t));

}

void push_back_vec_2d_int(ASM_vec_2d_int_t* p_vec, GLint* p_arr)
{
	void* xrealloc(void*, size_t);

	p_vec->ASM_pp_arr = (GLint**)xrealloc(p_vec->ASM_pp_arr, (p_vec->ASM_size + 1) * sizeof(GLint*));
	p_vec->ASM_size++;
	p_vec->ASM_pp_arr[p_vec->ASM_size - 1] = p_arr;

}

void push_back_vec_2d_float(ASM_vec_2d_float_t* p_vec, GLfloat* p_arr)
{
	void* xrealloc(void*, size_t);

	p_vec->ASM_pp_arr = (GLfloat**)xrealloc(p_vec->ASM_pp_arr, (p_vec->ASM_size + 1) * sizeof(GLfloat*));
	p_vec->ASM_size++;
	p_vec->ASM_pp_arr[p_vec->ASM_size - 1] = p_arr;

}

void clean_vec_2d_int(ASM_vec_2d_int_t** pp_vec)
{

	ASM_vec_2d_int_t* p_vec = *pp_vec;

	for (size_t i = 0; i != p_vec->ASM_size; i++)
		free(p_vec->ASM_pp_arr[i]);

	free(p_vec);
	*pp_vec = NULL;

}

void clean_vec_2d_float(ASM_vec_2d_float_t** pp_vec)
{

	ASM_vec_2d_float_t* p_vec = *pp_vec;

	for (size_t i = 0; i != p_vec->ASM_size; i++)
		free(p_vec->ASM_pp_arr[i]);

	//free(p_vec->ASM_pp_arr);
	free(p_vec);
	*pp_vec = NULL;

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

void* xrealloc(void *p, size_t new_size)
{
	void* ptr = realloc(p, new_size);
	if (!ptr)
	{
		fprintf(ASM_gpFile, "Realloc:Failed to allocate memory.");
		DestroyWindow(ASM_ghwnd);

	}

	return(ptr);
}
