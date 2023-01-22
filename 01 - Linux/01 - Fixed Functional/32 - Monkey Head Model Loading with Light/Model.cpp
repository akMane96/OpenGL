#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

#define ASM_BUFFER_SIZE 256

#define ASM_NR_POINT_COORDS 3
#define ASM_NR_TEXTURE_COORDS 2
#define ASM_NR_NORMAL_COORDS 3
#define ASM_NR_FACE_TOKENS 3
#define ASM_NR_TRIANGLE_VERTICES 3 


////Namespace
using namespace std;

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

////----------------------------------------------Global Varaiable Declaration

bool ASM_bFullScreen = false;
Display *ASM_gpDisplay = NULL;
XVisualInfo *ASM_gpVisualInfo = NULL;
Colormap ASM_gColormap;
Window ASM_gWIndow;
int ASM_gWindowWidth = 800;
int ASM_giWindowHeight = 600;
GLfloat ASM_angle;

GLXContext ASM_gGLXContext;

//-------------------------------------Model Loading Varaiable
ASM_vec_2d_float_t * ASM_gp_vertices = NULL;
ASM_vec_2d_float_t * ASM_gp_texture = NULL;
ASM_vec_2d_float_t* ASM_gp_normals = NULL;

ASM_vec_2d_int_t* ASM_gp_face_tri = NULL, * ASM_gp_face_texture = NULL, * ASM_gp_face_normals = NULL;

FILE* ASM_g_fp_meshfile = NULL;

char ASM_g_line[ASM_BUFFER_SIZE];

//-------------------------------------Lighting Varaiable
bool ASM_gbLight = false;
char ASM_key[26];

GLfloat ASM_lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat ASM_lightDiffuse[] = { 1.0f , 1.0f , 1.0f , 1.0f };
GLfloat ASM_lightPosition[] = { 0.0f, 0.0f, 100.0f, 1.0f };
GLfloat ASM_lightSpecular[] = { 1.0f , 1.0f , 1.0f , 1.0f };

GLfloat ASM_materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat ASM_materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat ASM_materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat ASM_materialShininess = 128.0f;


int main(void)
{
	void CreateWindow(void);
	void ToggleFullScreen(void);
	void Uninitialize(void);
	void Initialize(void);
	void Resize(int, int);
	void Render();	
	
	int ASM_winHeight = ASM_giWindowHeight;
	int ASM_winWidth = ASM_gWindowWidth;

	bool ASM_bDone = false;

	//code
	CreateWindow();
	Initialize();
	ToggleFullScreen();
	

	//MEssage Loop
	XEvent ASM_event;
	KeySym ASM_keysym;
	
	while(ASM_bDone == false)
	{
	
		while(XPending(ASM_gpDisplay))
		{
			XNextEvent(ASM_gpDisplay, &ASM_event);
			
			switch(ASM_event.type)
			{
			
				case MapNotify:
					break;
					
				case KeyPress:
					ASM_keysym = XkbKeycodeToKeysym(ASM_gpDisplay, ASM_event.xkey.keycode, 0, 0);
					switch(ASM_keysym)
					{
						case XK_Escape:
							ASM_bDone = true;
						case XK_f:
						case XK_F:
							
							if(ASM_bFullScreen == false)
							{
								ToggleFullScreen();	
								ASM_bFullScreen = true;
							}
							else
							{
								ToggleFullScreen();	
								ASM_bFullScreen = false;
							}						
							
							break;
						default:
							break;
					}
					XLookupString(&ASM_event.xkey, ASM_key, sizeof(ASM_key), NULL, NULL);
					
					switch(ASM_key[0])
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
						
					}
					
					break;
					
				case ButtonPress:
					switch(ASM_event.xbutton.button)
					{
						case 1:
							break;
						case 2:
							break;
						case 3:
							break;
						default:
							break;
					}
					break;	
					
				case MotionNotify:
					break;
				
				case ConfigureNotify:
					ASM_winWidth = ASM_event.xconfigure.width;
					ASM_winHeight = ASM_event.xconfigure.height;
					Resize(ASM_winWidth, ASM_winHeight);
					break;
					
				case Expose:
					break;
				case DestroyNotify:
					break;
					
				case 33:
					ASM_bDone = true;
				default:
					break;
						
			
			}	
		}
		
		Render();
		
		
	}
	
	Uninitialize();
	return(0);
}


void CreateWindow(void)
{
	void Uninitialize(void);
	

	
	//varaiable declaration
	XSetWindowAttributes ASM_winAttributes;
	int ASM_defaultScreen;
	int ASM_styleMask;
	static int ASM_frameBufferAttributes[] = 
					{GLX_DOUBLEBUFFER, True, 
					 GLX_RGBA, 
					 GLX_RED_SIZE, 8,
					 GLX_GREEN_SIZE, 8,
					 GLX_BLUE_SIZE, 8,
					 GLX_ALPHA_SIZE, 8,
					 GLX_DEPTH_SIZE, 24,
					 None};

	////code
	ASM_gpDisplay = XOpenDisplay(NULL);
	
	if(ASM_gpDisplay == NULL)
	{
		printf("ERROR: Unable To Open X Display........Exit \n");
		Uninitialize();
		exit(1);
	}
	
	ASM_defaultScreen = XDefaultScreen(ASM_gpDisplay);
	
	ASM_gpVisualInfo = glXChooseVisual(ASM_gpDisplay,ASM_defaultScreen, ASM_frameBufferAttributes);
	
	ASM_winAttributes.border_pixel = 0;
	ASM_winAttributes.background_pixmap = 0;
	ASM_winAttributes.colormap = XCreateColormap(ASM_gpDisplay,
							RootWindow(ASM_gpDisplay, ASM_gpVisualInfo ->screen), ASM_gpVisualInfo->visual, AllocNone);
							
	ASM_gColormap = ASM_winAttributes.colormap;
	
	ASM_winAttributes.background_pixel = BlackPixel(ASM_gpDisplay, ASM_defaultScreen);
	ASM_winAttributes.event_mask = ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;
	
	ASM_styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	
	ASM_gWIndow = XCreateWindow (ASM_gpDisplay,
					RootWindow(ASM_gpDisplay, ASM_gpVisualInfo->screen),
					0,
					0,
					ASM_gWindowWidth,
					ASM_giWindowHeight,
					0,
					ASM_gpVisualInfo->depth,
					InputOutput,
					ASM_gpVisualInfo->visual,
					ASM_styleMask,
					&ASM_winAttributes);
	if(!ASM_gWIndow)
	{
		printf("ERROR: Failed to Create Main Window\n");
		Uninitialize();
		exit(1);	
	}
	
	XStoreName(ASM_gpDisplay, ASM_gWIndow, "Model Loading Monkey Head");
	
	Atom ASM_windowManagerDelete = XInternAtom(ASM_gpDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(ASM_gpDisplay,ASM_gWIndow, &ASM_windowManagerDelete,1 );
	
	XMapWindow(ASM_gpDisplay, ASM_gWIndow);				
					
	
}

void ToggleFullScreen(void)
{
	Atom ASM_wm_state;
	Atom ASM_fullscreen;
	XEvent ASM_xev = {0};
	
	//code
	ASM_wm_state = XInternAtom(ASM_gpDisplay, "_NET_WM_STATE", False);
	memset(&ASM_xev, 0, sizeof(ASM_xev));
	
	ASM_xev.type = ClientMessage;
	ASM_xev.xclient.window = ASM_gWIndow;
	ASM_xev.xclient.message_type = ASM_wm_state;
	ASM_xev.xclient.format = 32;
	ASM_xev.xclient.data.l[0] = ASM_bFullScreen ? 0:1;
	
	ASM_fullscreen = XInternAtom(ASM_gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
	ASM_xev.xclient.data.l[1] = ASM_fullscreen;
	
	XSendEvent(ASM_gpDisplay,
		RootWindow(ASM_gpDisplay, ASM_gpVisualInfo->screen),
		False,
		StructureNotifyMask,
		&ASM_xev);

}

void Initialize(void){

	void Resize(int, int);
	void LoadMeshData();
	
	ASM_gGLXContext = glXCreateContext(ASM_gpDisplay, ASM_gpVisualInfo, NULL, GL_TRUE);
	
	glXMakeCurrent(ASM_gpDisplay, ASM_gWIndow, ASM_gGLXContext);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, ASM_lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, ASM_lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, ASM_lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, ASM_lightPosition);

	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glEnable(GL_LIGHT0);
	
	
	LoadMeshData();
	
	Resize(ASM_gWindowWidth, ASM_giWindowHeight);
	
}

void Resize(int width, int height){

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
	
	void Uninitialize(void);
	
	ASM_vec_2d_int_t* create_vec_2d_int(void);
	ASM_vec_2d_float_t* create_vec_2d_float(void);
	void push_back_vec_2d_int(ASM_vec_2d_int_t *, int*);
	void push_back_vec_2d_float(ASM_vec_2d_float_t *, float*);
	void* xcalloc(int, size_t);

	ASM_g_fp_meshfile = fopen("MonkeyHead.obj", "r");

	if(!ASM_g_fp_meshfile)
	{
		printf("Failed to load");
		Uninitialize();
		exit(1);
		
	}

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
	
	printf("gp_vertices size : %zu gp_texture size : %zu gp_normasl: %zu gp_face_tri :%zu\n\n", ASM_gp_vertices->ASM_size, ASM_gp_texture->ASM_size, ASM_gp_normals->ASM_size, ASM_gp_face_tri->ASM_size);

	
}

void Render()
{
	void Update();

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	
	glTranslatef(0.0f, 0.0f, -5.0f);
	glRotatef(ASM_angle, 0.0f, 1.0f,0.0f);
	glScalef(1.5f, 1.5f, 1.5f);

	glFrontFace(GL_CCW);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (int i = 0; i != ASM_gp_face_tri->ASM_size; i++)
	{

		glBegin(GL_TRIANGLES);
			for (int j = 0; j != ASM_NR_TRIANGLE_VERTICES; j++)
			{
				int vi = ASM_gp_face_tri->ASM_pp_arr[i][j] - 1;
				int ni = ASM_gp_face_normals->ASM_pp_arr[i][j] - 1;
				glNormal3f(ASM_gp_normals->ASM_pp_arr[ni][0], ASM_gp_normals->ASM_pp_arr[ni][1], ASM_gp_normals->ASM_pp_arr[ni][2]);
				glVertex3f(ASM_gp_vertices->ASM_pp_arr[vi][0], ASM_gp_vertices->ASM_pp_arr[vi][1], ASM_gp_vertices->ASM_pp_arr[vi][2]);

			}

		glEnd();
	}
	
	Update();
	glXSwapBuffers(ASM_gpDisplay, ASM_gWIndow);

}


void Update()
{
	ASM_angle = ASM_angle + 0.5f;
	if(ASM_angle > 360.0f)
	{
		ASM_angle = 0.0f;
		
	}
}

void Uninitialize(void)
{

	void clean_vec_2d_float(ASM_vec_2d_float_t**);
	void clean_vec_2d_int(ASM_vec_2d_int_t**);
	
	
	GLXContext ASM_currentGLXCOntext;
	ASM_currentGLXCOntext = glXGetCurrentContext();
	
	clean_vec_2d_float(&ASM_gp_vertices);
	clean_vec_2d_float(&ASM_gp_texture);
	clean_vec_2d_float(&ASM_gp_normals);

	clean_vec_2d_int(&ASM_gp_face_tri);
	clean_vec_2d_int(&ASM_gp_face_texture);
	clean_vec_2d_int(&ASM_gp_face_normals);
	
	if(ASM_currentGLXCOntext == ASM_gGLXContext)
	{
		glXGetCurrentContext();
	}
	if(ASM_gGLXContext)
	{
		glXDestroyContext(ASM_gpDisplay, ASM_gGLXContext);
	}
	
	if(ASM_gWIndow)
	{
		XDestroyWindow(ASM_gpDisplay, ASM_gWIndow);
	}
	
	if(ASM_gColormap)
	{
		XFreeColormap(ASM_gpDisplay, ASM_gColormap);
	}
	if(ASM_gpVisualInfo)
	{
		free(ASM_gpVisualInfo);
		ASM_gpVisualInfo = NULL;
	}
	
	if(ASM_gpDisplay)
	{
		XCloseDisplay(ASM_gpDisplay);
		ASM_gpDisplay = NULL;
	}
	
	
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
	void Uninitialize(void);
	
	void* p = calloc(nr_elements, size_per_elements);

	if (!p)
	{
		printf("Calloc:Failed to allocate memory.");
		Uninitialize();
		exit(1);

	}

	return(p);
}

void* xrealloc(void *p, size_t new_size)
{
	void Uninitialize(void);
	
	void* ptr = realloc(p, new_size);
	if (!ptr)
	{
		printf("Realloc:Failed to allocate memory.");
		Uninitialize();
		exit(1);

	}

	return(ptr);
}

