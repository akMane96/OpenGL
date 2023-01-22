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

#include<SOIL/SOIL.h>
#include "TeaPot.h"


////Namespace
using namespace std;

////Global Varaiable Declaration

bool ASM_bFullScreen = false;
Display *ASM_gpDisplay = NULL;
XVisualInfo *ASM_gpVisualInfo = NULL;
Colormap ASM_gColormap;
Window ASM_gWIndow;
int ASM_gWindowWidth = 800;
int ASM_giWindowHeight = 600;

GLXContext ASM_gGLXContext;

char ASM_key[26];

GLfloat ASM_angle;
bool ASM_gbAnimate = false;
bool ASM_gbLight = false;
bool ASM_gbTexture = false;
GLuint ASM_Marble_texture;

GLfloat ASM_lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat ASM_lightDiffuse[] = { 1.0f , 1.0f , 1.0f , 1.0f };
GLfloat ASM_lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };
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

						case 't':
						case 'T':
							if (ASM_gbTexture == true)
							{
								ASM_gbTexture = false;
							}
							else if (ASM_gbTexture == false)
							{
								ASM_gbTexture = true;
							}

							if (ASM_gbTexture == true)
							{
								glEnable(GL_TEXTURE_2D);
							}
							else
							{
								glDisable(GL_TEXTURE_2D);
							}
							break;

						case 'a':
						case 'A':

							if (ASM_gbAnimate == true)
							{
								ASM_gbAnimate = false;
							}
							else if (ASM_gbAnimate == false)
							{
								ASM_gbAnimate = true;
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
	
	XStoreName(ASM_gpDisplay, ASM_gWIndow, "TeaPot with Static Rotation");
	
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
	GLuint loadBitmapAsTexture(const char *);
	
	ASM_gGLXContext = glXCreateContext(ASM_gpDisplay, ASM_gpVisualInfo, NULL, GL_TRUE);
	
	glXMakeCurrent(ASM_gpDisplay, ASM_gWIndow, ASM_gGLXContext);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	
	ASM_Marble_texture = loadBitmapAsTexture("marble.bmp");
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, ASM_lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, ASM_lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, ASM_lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, ASM_lightPosition);

	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glEnable(GL_LIGHT0);
	
		
	Resize(ASM_gWindowWidth, ASM_giWindowHeight);
	
}

GLuint loadBitmapAsTexture(const char *path)
{
	
	//Varaible decraltion
	int ASM_width, ASM_height;
	unsigned char *ASM_imageData = NULL;
	GLuint textureId;
	
	
	ASM_imageData = SOIL_load_image(path, &ASM_width, &ASM_height, NULL, SOIL_LOAD_RGB);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, ASM_width, ASM_height, GL_RGB, GL_UNSIGNED_BYTE, ASM_imageData);
	
	SOIL_free_image_data(ASM_imageData);
	
	return textureId;

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

void Render()
{

	void Update();

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -1.0f);
	glRotatef(ASM_angle, 0.0f, 1.0f, 0.0f);
		
	glBindTexture(GL_TEXTURE_2D, ASM_Marble_texture);

	//front triangle
	glBegin(GL_TRIANGLES);
	for (int ASM_i = 0; ASM_i < sizeof(face_indicies) / sizeof(face_indicies[0]); ASM_i++)
	{
		for (int ASM_j = 0; ASM_j < 3; ASM_j++)
		{
			int ASM_vi = face_indicies[ASM_i][ASM_j];
			int ASM_ni = face_indicies[ASM_i][ASM_j + 3];
			int ASM_ti = face_indicies[ASM_i][ASM_j + 6];

			glNormal3f(normals[ASM_ni][0], normals[ASM_ni][1], normals[ASM_ni][1]);
			glTexCoord2f(textures[ASM_ti][0], textures[ASM_ti][1]);
			glVertex3f(vertices[ASM_vi][0], vertices[ASM_vi][1], vertices[ASM_vi][2]);
		}
	}
	glEnd();

	if (ASM_gbAnimate == true)
	{
		Update();
	}

	
	glXSwapBuffers(ASM_gpDisplay, ASM_gWIndow);

}

void Update()
{
	ASM_angle = ASM_angle + 0.5f;

	if (ASM_angle > 360.0f)
		ASM_angle = 0.0f;

}

void Uninitialize(void)
{
	GLXContext ASM_currentGLXCOntext;
	ASM_currentGLXCOntext = glXGetCurrentContext();
	
	if (ASM_Marble_texture)
	{
		glDeleteTextures(1, &ASM_Marble_texture);
	}
	
	
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

