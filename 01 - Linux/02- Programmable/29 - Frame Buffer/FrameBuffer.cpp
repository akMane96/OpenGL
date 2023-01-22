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

using namespace vmath;

enum
{
	ASM_ATTRIBUTE_POSITION = 0,
	ASM_ATTRIBUTE_COLOR,
	ASM_ATTRIBUTE_NORMAL,
	ASM_ATTRIBUTE_TEXTURECOORD,

};

#define  ASM_FRAMEBUFFER_WIDTH 200
#define ASM_FRAMEBUFFER_HEIGHT 200


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
	
	XStoreName(ASM_gpDisplay, ASM_gWindow, "Perspective Coloured Triangle");
	
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
	void createFramebuffer();
	
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
				Uninitialize();
				exit(1);
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
				Uninitialize();
				exit(1);
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
				Uninitialize();
				exit(1);
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
				Uninitialize();
				exit(1);
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
				Uninitialize();
				exit(1);
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
				Uninitialize();
				exit(1);
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


	ASM_perspectiveProjectionMatrix = mat4::identity();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


	createFramebuffer();
		
	Resize(ASM_gWindowWidth, ASM_giWindowHeight);
	
	
	

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

void Resize(int width, int height){

	if(height <= 0)
		height = 1;
		
	ASM_WIN_WIDTH = width;
	ASM_WIN_HEIGHT = height;
	
	
}
void Render(){

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
		
	glXSwapBuffers(ASM_gpDisplay, ASM_gWindow);

}


void Update()
{
	ASM_CubeAngle = ASM_CubeAngle + 0.5f;

	if (ASM_CubeAngle >= 360.0f)
	{
		ASM_CubeAngle = 0.0f;
	}

}

void Uninitialize(){

	GLXContext ASM_currentGLXContext;
	ASM_currentGLXContext = glXGetCurrentContext();
	
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




























