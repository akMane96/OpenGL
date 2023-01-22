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
	
	XStoreName(ASM_gpDisplay, ASM_gWindow, "Graph Paper with Shapes");
	
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
				Uninitialize();
				exit(1);
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


	ASM_perspectiveProjectionMatrix = mat4::identity();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
		
	Resize(ASM_gWindowWidth, ASM_giWindowHeight);
	
	
	

}
void Resize(int width, int height){

	if(height <= 0)
		height = 1;
		
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);	

	ASM_perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width/ (GLfloat)height, 0.1f, 100.0f);
	
	
}
void Render(){

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
		
	glXSwapBuffers(ASM_gpDisplay, ASM_gWindow);

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




























