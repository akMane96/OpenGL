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
#include "Sphere.h"

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
FILE * ASM_gpFile = NULL;

/////---------------------------------------Context Varaibles
GLXContext ASM_gGLXContext;

typedef GLXContext (* glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int *);
glXCreateContextAttribsARBProc ASM_glXCreateContextAttribsARB = NULL;
GLXFBConfig ASM_gGLXFBConfig;

/////---------------------------------------_Shader Varaibles

GLfloat ASM_angle;

/////-------------------------------------------- Vertex and Shader Object
GLuint ASM_vertexShaderObject;
GLuint ASM_fragmentShaderObject;
GLuint ASM_shaderProgramObject;

/////-------------------------------------------- VAO and VBO
GLuint ASM_vao;
GLuint ASM_vbo;
GLuint ASM_vbo_sphere_position;
GLuint ASM_vbo_sphere_normals;
GLuint ASM_vbo_sphere_element;

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
vec4 ASM_lightPositionVector = { 100.0f, 100.0f, 100.0f, 1.0f };

/////-------------------------------------------- Material Uniform

GLuint ASM_kAUniform;
GLuint ASM_kDUniform;
GLuint ASM_kSUniform;
GLuint ASM_materialShininessUniform;

/////-------------------------------------------- Material Arrays

vec3 ASM_kAVector = { 0.0f, 0.0f, 0.0f };
vec3 ASM_kDVector = { 0.5f, 0.2f , 0.7f };
vec3 ASM_kSVector = { 0.7f, 0.7f , 0.7f };
GLfloat ASM_materialShininess = 128.0f;


/////-------------------------------------------- Sphere Variables
GLfloat ASM_sphere_vertices[1146];
GLfloat ASM_sphere_normals[1146];
GLfloat ASM_sphere_textures[764];
unsigned short ASM_sphere_elements[2280];
GLuint ASM_gNumVertices;
GLuint ASM_gNumElements;


/////-------------------------------------------- Projection Matrix
mat4 ASM_perspectiveProjectionMatrix;


/////-------------------------------------------- Other
GLuint ASM_lKeyPressedUniform;
bool ASM_bLight;

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

						case XK_l:
						case XK_L:
							if (ASM_bLight == false)
							{
								ASM_bLight = true;
								
							}
							else
							{
								ASM_bLight = false;
								
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
	
	XStoreName(ASM_gpDisplay, ASM_gWindow, "Per Vertex Lighting on Sphere");
	
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
	
	
	////-----------------------------------------------------------------------------------------Vertex Shader

	//Create Shader
	ASM_vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide source code

	const GLchar* ASM_vertexShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"in vec4 ASM_vPosition;" \
		"in vec3 ASM_vNormal;" \
		"uniform mat4 ASM_u_modelMatrix;" \
		"uniform mat4 ASM_u_viewpMatrix;" \
		"uniform mat4 ASM_u_projectionMatrix;" \
		"uniform vec3 ASM_u_lA;" \
		"uniform vec3 ASM_u_lD;" \
		"uniform vec3 ASM_u_lS;" \
		"uniform vec4 ASM_u_lightPosition;" \
		"uniform vec3 ASM_u_kA;" \
		"uniform vec3 ASM_u_kD;" \
		"uniform vec3 ASM_u_kS;" \
		"uniform float ASM_u_Shininess;" \
		"uniform int ASM_u_lKeyPressed;" \
		"out vec3 ASM_Phong_ADS_Light;" \
		"void main(void)" \
		"{" \
			"if(ASM_u_lKeyPressed == 1)" \
			"{" \
				"vec4 ASM_eye_coordinates =  ASM_u_viewpMatrix * ASM_u_modelMatrix *  ASM_vPosition;" \
				"vec3 ASM_transformedNormal =  normalize(mat3(ASM_u_viewpMatrix * ASM_u_modelMatrix) * ASM_vNormal);" \
				"vec3 ASM_lightDirection =  normalize(vec3(ASM_u_lightPosition - ASM_eye_coordinates)) ;" \
				"vec3 ASM_reflectionVector = reflect(-ASM_lightDirection, ASM_transformedNormal);" \
				"vec3 ASM_viewerVector = normalize(-(ASM_eye_coordinates.xyz));" \
				"vec3 ASM_ambient =  ASM_u_lA * ASM_u_kA;" \
				"vec3 ASM_diffuse =  ASM_u_lD * ASM_u_kD * max(dot(ASM_lightDirection, ASM_transformedNormal), 0.0f);" \
				"vec3 ASM_specular =  ASM_u_lS * ASM_u_kS * pow(max(dot(ASM_reflectionVector, ASM_viewerVector), 0.0f), ASM_u_Shininess);" \
				"ASM_Phong_ADS_Light =  ASM_ambient + ASM_diffuse + ASM_specular;" \

			"}" \
			"else" \
			"{" \
				"ASM_Phong_ADS_Light = vec3(1.0f, 1.0f, 1.0f);" \
			"}" \
			
			"gl_Position = ASM_u_projectionMatrix * ASM_u_viewpMatrix * ASM_u_modelMatrix * ASM_vPosition;" \
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
		////---------------------------------------------------------------------------Fragment Shader

	ASM_fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar *ASM_fragmentShaderSourceCode = 
		"#version 450 core" \
		"\n" \
		"in vec3 ASM_Phong_ADS_Light;" \
		"out vec4 ASM_fragColor;" \
		"void main(void)" \
		"{" \
			"ASM_fragColor = vec4(ASM_Phong_ADS_Light, 1.0f);" \
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

	// ----------------------------------------------------------------------------------------------- Shader Program

	ASM_shaderProgramObject = glCreateProgram();

	glAttachShader(ASM_shaderProgramObject, ASM_vertexShaderObject);

	glAttachShader(ASM_shaderProgramObject, ASM_fragmentShaderObject);

	glBindAttribLocation(ASM_shaderProgramObject, ASM_ATTRIBUTE_POSITION, "ASM_vPosition");

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
				Uninitialize();
				exit(1);
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


	getSphereVertexData(ASM_sphere_vertices, ASM_sphere_normals, ASM_sphere_textures, ASM_sphere_elements);
	ASM_gNumVertices = getNumberOfSphereVertices();
	ASM_gNumElements = getNumberOfSphereElements();


	glGenVertexArrays(1, &ASM_vao);
	glBindVertexArray(ASM_vao);

		glGenBuffers(1, &ASM_vbo_sphere_position);
			glBindBuffer(GL_ARRAY_BUFFER, ASM_vbo_sphere_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_sphere_vertices), ASM_sphere_vertices, GL_STATIC_DRAW);
	
			glVertexAttribPointer(ASM_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			glEnableVertexAttribArray(ASM_ATTRIBUTE_POSITION);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &ASM_vbo_sphere_normals);
			glBindBuffer(GL_ARRAY_BUFFER, ASM_vbo_sphere_normals);
			glBufferData(GL_ARRAY_BUFFER, sizeof(ASM_sphere_normals), ASM_sphere_normals, GL_STATIC_DRAW);

			glVertexAttribPointer(ASM_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			glEnableVertexAttribArray(ASM_ATTRIBUTE_NORMAL);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &ASM_vbo_sphere_element);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ASM_vbo_sphere_element);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ASM_sphere_elements), ASM_sphere_elements, GL_STATIC_DRAW);

	
		glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);


	ASM_perspectiveProjectionMatrix = mat4::identity();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

			
	Resize(ASM_gWindowWidth, ASM_giWindowHeight);
	
	

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

		mat4 ASM_modelMatrix = mat4::identity();
		mat4 ASM_viewMatrix = mat4::identity();
		mat4 ASM_translateMatrix = mat4::identity();
		mat4 ASM_scaleMatrix = mat4::identity();
		mat4 ASM_rotateMatrix = mat4::identity();

		ASM_translateMatrix = translate(0.0f, 0.0f, -3.0f);
		ASM_scaleMatrix = scale(2.0f, 2.0f, 2.0f);
		ASM_rotateMatrix = rotate(ASM_angle, 0.0f, 1.0f,0.0f);

		ASM_modelMatrix = ASM_translateMatrix * ASM_scaleMatrix * ASM_rotateMatrix;
		

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

		glBindVertexArray(ASM_vao);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ASM_vbo_sphere_element);
			glDrawElements(GL_TRIANGLES, ASM_gNumElements, GL_UNSIGNED_SHORT, 0);

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

	if (ASM_vbo_sphere_position)
	{
		glDeleteBuffers(1, &ASM_vbo_sphere_position);
		ASM_vbo_sphere_position = 0;
	}

	if (ASM_vbo_sphere_normals)
	{
		glDeleteBuffers(1, &ASM_vbo_sphere_normals);
		ASM_vbo_sphere_normals = 0;
	}

	if (ASM_vbo_sphere_element)
	{
		glDeleteBuffers(1, &ASM_vbo_sphere_element);
		ASM_vbo_sphere_element = 0;
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




























