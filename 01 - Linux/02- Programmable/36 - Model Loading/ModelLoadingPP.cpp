#include<iostream>
#include<stdio.h>
#include <stdlib.h>
#include<memory.h>


#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

#include<GL/glew.h>
#include<GL/gl.h>
#include<GL/glx.h>

#include "vmath.h"
#define ASM_BUFFER_SIZE 256

#define ASM_NR_POINT_COORDS 3
#define ASM_NR_TEXTURE_COORDS 2
#define ASM_NR_NORMAL_COORDS 3
#define ASM_NR_FACE_TOKENS 3
#define ASM_NR_TRIANGLE_VERTICES 3 

using namespace vmath;

enum
{
	ASM_ATTRIBUTE_POSITION = 0,
	ASM_ATTRIBUTE_COLOR,
	ASM_ATTRIBUTE_NORMAL,
	ASM_ATTRIBUTE_TEXTURECOORD,

};

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


using namespace std;

bool ASM_bFullScreen = false;
Display *ASM_gpDisplay = NULL;
XVisualInfo *ASM_gpVisualInfo = NULL;
Colormap ASM_gColormap;
Window ASM_gWindow;
int ASM_gWindowWidth = 800;
int ASM_giWindowHeight = 600;
GLfloat ASM_angle;
FILE * ASM_gpFile = NULL;

/////---------------------------------------Context Varaibles
GLXContext ASM_gGLXContext;

typedef GLXContext (* glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int *);
glXCreateContextAttribsARBProc ASM_glXCreateContextAttribsARB = NULL;
GLXFBConfig ASM_gGLXFBConfig;

/////---------------------------------------_Shader Varaibles

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

int main(void)
{

	void CreateWindow();
	void ToggleFullScreen();
	void Uninitialize();
	void Initialize();
	void Resize(int, int);
	void Render();
	
	
	int ASM_winWidth = ASM_gWindowWidth;
	int ASM_winHeight =  ASM_giWindowHeight;
	
	bool ASM_bDone = false;
	
	
	/////----------------------File
	
	ASM_gpFile = fopen("ASMLog.txt", "w");
	
	if(ASM_gpFile != NULL)
	{
		fprintf(ASM_gpFile, "Program Execution Started Succesfully\n\n");
			
	} else
	{
		exit(0);
	
	}
	
	
	CreateWindow();
	Initialize();
	ToggleFullScreen();
	
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
					ASM_keysym = XkbKeycodeToKeysym(ASM_gpDisplay,ASM_event.xkey.keycode,0,0);
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

void CreateWindow()
{
	void Uninitialize();
	
	XSetWindowAttributes ASM_WinAttr;
	GLXFBConfig *ASM_pGLXFBConfig = NULL;
	GLXFBConfig ASM_bestGLXFBConfig;
	XVisualInfo * ASM_pTempXVisualInfo;
	int ASM_numFBConfigs = 0;
	int ASM_defaultScreen;
	int ASM_styleMask;
	
	static int ASM_FrameBufferAttributes[] = 
						{ GLX_DOUBLEBUFFER, True,
						  GLX_X_RENDERABLE, True,
						  GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
						  GLX_RENDER_TYPE, GLX_RGBA_BIT,
						  GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
						  GLX_RED_SIZE, 8,
						  GLX_GREEN_SIZE, 8,
						  GLX_BLUE_SIZE, 8,
						  GLX_ALPHA_SIZE, 8,
						  GLX_STENCIL_SIZE, 8,
						  GLX_DEPTH_SIZE, 24,
						  None						
						};
						
	ASM_gpDisplay = XOpenDisplay(NULL);
	
	if(ASM_gpDisplay == NULL)
	{
		fprintf(ASM_gpFile,"ERROR: Unable To Open X Display...Exit \n");
		Uninitialize();
		exit(1);
	}
	
	ASM_defaultScreen = XDefaultScreen(ASM_gpDisplay);
	
	//ASM_gpVisualInfo = glXChooseVisual(ASM_gpDisplay, ASM_defaultScreen, ASM_FrameBufferAttributes);
	
	ASM_pGLXFBConfig = glXChooseFBConfig(ASM_gpDisplay, ASM_defaultScreen, ASM_FrameBufferAttributes, &ASM_numFBConfigs);
	
	fprintf(ASM_gpFile, " -------------------------------- FBConfig--------------------------------------\n\n");
	
	fprintf(ASM_gpFile,"Found Number of FBConfig :   %d\n\n",ASM_numFBConfigs);
	
	int ASM_bestFrameBufferConfig = -1;
	int ASM_worstFrameBufferConfig = -1;
	int ASM_bestNumberOfSamples = -1;
	int ASM_worstNumberOfSamples = 999;
	
	for(int i = 0; i < ASM_numFBConfigs; i++)
	{
		
		ASM_pTempXVisualInfo = glXGetVisualFromFBConfig(ASM_gpDisplay, ASM_pGLXFBConfig[i]);
		
		if(ASM_pTempXVisualInfo != NULL)
		{
			int ASM_sampleBuffers, ASM_samples;
			
			glXGetFBConfigAttrib(ASM_gpDisplay, ASM_pGLXFBConfig[i], GLX_SAMPLE_BUFFERS, &ASM_sampleBuffers);
			glXGetFBConfigAttrib(ASM_gpDisplay, ASM_pGLXFBConfig[i], GLX_SAMPLES, &ASM_samples);
			
			if(ASM_bestFrameBufferConfig < 0 || ASM_sampleBuffers && ASM_samples > ASM_bestNumberOfSamples)
			{
				ASM_bestFrameBufferConfig = i;
				ASM_bestNumberOfSamples = ASM_samples;
			}
			
			if(ASM_worstFrameBufferConfig < 0 || !ASM_sampleBuffers || ASM_samples < ASM_worstNumberOfSamples)
			{
				ASM_worstFrameBufferConfig = i;
				ASM_worstNumberOfSamples = ASM_samples;
			}
			
			
			fprintf(ASM_gpFile,"When i = %d , samples = %d , sample Buffers = %d,  ASM_pTempXVisualInfo Id = %ld\n", i, ASM_samples, ASM_sampleBuffers, ASM_pTempXVisualInfo->visualid);
			
		}
		
		XFree(ASM_pTempXVisualInfo);
	
	}
	fprintf(ASM_gpFile, " ------------------------------------------------------------------------------------------------\n");
	
	ASM_bestGLXFBConfig = ASM_pGLXFBConfig[ASM_bestFrameBufferConfig];
	ASM_gGLXFBConfig = ASM_bestGLXFBConfig;
	
	XFree(ASM_pGLXFBConfig);
	
	ASM_gpVisualInfo = glXGetVisualFromFBConfig(ASM_gpDisplay, ASM_gGLXFBConfig);
	
	ASM_WinAttr.border_pixel = 0;
	ASM_WinAttr.background_pixmap = 0;
	ASM_WinAttr.colormap = XCreateColormap(ASM_gpDisplay, RootWindow(ASM_gpDisplay, ASM_gpVisualInfo->screen), ASM_gpVisualInfo->visual, AllocNone);
	
	ASM_gColormap = ASM_WinAttr.colormap;
	
	ASM_WinAttr.background_pixel = BlackPixel(ASM_gpDisplay, ASM_defaultScreen);
	ASM_WinAttr.event_mask = ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;
	
	ASM_styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	
	ASM_gWindow = XCreateWindow (
					ASM_gpDisplay,
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
					&ASM_WinAttr	
					);
	
	if(!ASM_gWindow)
	{
		fprintf(ASM_gpFile,"ERROR: Failed to Create Main Window\n");
		Uninitialize();
		exit(1);	
	}
	
	XStoreName(ASM_gpDisplay, ASM_gWindow, "Plain Sphere");
	
	Atom ASM_windowManagerDelete = XInternAtom(ASM_gpDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(ASM_gpDisplay, ASM_gWindow, &ASM_windowManagerDelete, 1);
	
	XMapWindow(ASM_gpDisplay, ASM_gWindow);	
	
		
}


void ToggleFullScreen()
{
	
	Atom ASM_wm_state;
	Atom ASM_fullscreen;
	XEvent ASM_xev = {0};
	
	ASM_wm_state = XInternAtom(ASM_gpDisplay, "_NET_WM_STATE", False);
	memset(&ASM_xev, 0 , sizeof(ASM_xev));
	
	ASM_xev.type = ClientMessage;
	ASM_xev.xclient.window = ASM_gWindow;
	ASM_xev.xclient.message_type = ASM_wm_state;
	ASM_xev.xclient.format = 32;
	ASM_xev.xclient.data.l[0]= ASM_bFullScreen ? 0:1;
	
	ASM_fullscreen = XInternAtom(ASM_gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
	ASM_xev.xclient.data.l[1] = ASM_fullscreen;
	
	XSendEvent(ASM_gpDisplay, RootWindow(ASM_gpDisplay, ASM_gpVisualInfo->screen), False,StructureNotifyMask,&ASM_xev);

}

void Initialize(){

	void Resize(int, int);
	void Uninitialize();
	void LoadMeshData();
	
	ASM_glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte*)"glXCreateContextAttribsARB");
	
	int ASM_attribs[] = { GLX_CONTEXT_MAJOR_VERSION_ARB, 4, 
				    GLX_CONTEXT_MINOR_VERSION_ARB, 5,
				    GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
				    None };
	
	ASM_gGLXContext = ASM_glXCreateContextAttribsARB(ASM_gpDisplay, ASM_gGLXFBConfig, 0, True,  ASM_attribs);
	
	if(!ASM_gGLXContext)
	{
	
		int ASM_attribs[] = { GLX_CONTEXT_MAJOR_VERSION_ARB, 1, 
				  GLX_CONTEXT_MINOR_VERSION_ARB, 0,
				  None };
				  
		ASM_gGLXContext = ASM_glXCreateContextAttribsARB(ASM_gpDisplay, ASM_gGLXFBConfig, 0, True,  ASM_attribs);
	
	}		
	
	Bool ASM_bIsDirectContext;
	
	ASM_bIsDirectContext = glXIsDirect(ASM_gpDisplay, ASM_gGLXContext);
	
	fprintf(ASM_gpFile,"\n\n--------------------------- Context Details -------------------------------\n\n");
	if(ASM_bIsDirectContext = True)
	{
		fprintf(ASM_gpFile,"Rendering Context is Direct Hardware Rendering Context\n\n");
	
	}
	else
	{
		fprintf(ASM_gpFile,"Rendering Context is not Direct Hardware Rendering Context but Software Renderig Context\n\n");
	}
	fprintf(ASM_gpFile, " ------------------------------------------------------------------------------------------------\n");
	
	glXMakeCurrent(ASM_gpDisplay, ASM_gWindow, ASM_gGLXContext);
	
	GLenum ASM_glew_error = glewInit();

	if (ASM_glew_error != GLEW_OK)
	{
		fprintf(ASM_gpFile, "glewInit(): Failed !!!!!\n");
		Uninitialize();
		exit(1);

	}	
	
	// Graphic Library Shading Language
	///// OpenGL Related Log
	fprintf(ASM_gpFile, "\n------------------- OpenGL Related Log ---------------------\n\n");
	fprintf(ASM_gpFile, "OpenGL Vendor : %s\n\n", glGetString(GL_VENDOR));
	fprintf(ASM_gpFile, "OpenGL Renderer : %s\n\n", glGetString(GL_RENDERER));
	fprintf(ASM_gpFile, "OpenGL Version : %s\n\n", glGetString(GL_VERSION));
	fprintf(ASM_gpFile, "GLSL Version : %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	//OpenGL Enabled Extension
	fprintf(ASM_gpFile, "\n------------------- OpenGL Enabled Extension ---------------------\n\n");
	fflush(ASM_gpFile);
	GLint ASM_numExt;

	glGetIntegerv(GL_NUM_EXTENSIONS, &ASM_numExt);

	for (int ASM_i = 0; ASM_i < ASM_numExt; ASM_i++)
	{
		fprintf(ASM_gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, ASM_i));
		fflush(ASM_gpFile);
	}

	fprintf(ASM_gpFile, "\n\n------------------------------------------------------\n\n");
	
	
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
				Uninitialize();
				exit(1);
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
				Uninitialize();
				exit(1);
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
				Uninitialize();
				exit(1);
			}

		}

	}

	ASM_mvpUniform = glGetUniformLocation(ASM_shaderProgramObject, "ASM_u_mvpMatrix");
	
	LoadMeshData();

	
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


	ASM_perspectiveProjectionMatrix = mat4::identity();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

			
	Resize(ASM_gWindowWidth, ASM_giWindowHeight);
	
	

}

void LoadMeshData()
{
	void Uninitialize();

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
		Uninitialize();
		exit(1);
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

void Resize(int width, int height){

	if(height <= 0)
		height = 1;
		
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);	

	ASM_perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width/ (GLfloat)height, 0.1f, 100.0f);
	
	
}
void Render(){

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ASM_shaderProgramObject);

		mat4 ASM_modelViewMatrix = mat4::identity();
		mat4 ASM_modelViewProjectMatrix = mat4::identity();
		mat4 ASM_translateMatrix = mat4::identity();
		mat4 ASM_rotateMatrix = mat4::identity();

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


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
		
	glXSwapBuffers(ASM_gpDisplay, ASM_gWindow);

}

void Uninitialize(){

	GLXContext ASM_currentGLXContext;
	ASM_currentGLXContext = glXGetCurrentContext();
	
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

	//---------------------------------------------------------Context	
	if(ASM_currentGLXContext == ASM_gGLXContext)
	{
		glXGetCurrentContext();
	}
	if(ASM_gGLXContext)
	{
		glXDestroyContext(ASM_gpDisplay,ASM_gGLXContext);
	}
	
	if(ASM_gWindow)
	{
		XDestroyWindow(ASM_gpDisplay,ASM_gWindow);
	}
	
	if(ASM_gColormap)
	{
		XFreeColormap(ASM_gpDisplay,ASM_gColormap);
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
	
	if(ASM_gpFile != NULL)
	{
		fprintf(ASM_gpFile, "Program Execution Ended Succesfully\n");
		
		fclose(ASM_gpFile);
		ASM_gpFile =NULL;
			
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
		Uninitialize();
exit(1);

	}

	return(p);
}

void* xrealloc(void* p, size_t new_size)
{
	void* ptr = realloc(p, new_size);
	if (!ptr)
	{
		fprintf(ASM_gpFile, "Realloc:Failed to allocate memory.");
		Uninitialize();
exit(1);

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




























