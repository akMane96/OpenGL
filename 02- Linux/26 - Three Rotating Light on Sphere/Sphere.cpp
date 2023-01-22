#include<iostream>
#include<stdio.h>
#include <stdlib.h>
#include<memory.h>


#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

using namespace std;

bool ASM_bFullScreen = false;
Display *ASM_gpDisplay = NULL;
XVisualInfo *ASM_gpVisualInfo = NULL;
Colormap ASM_gColormap;
Window ASM_gWindow;
int ASM_gWindowWidth = 800;
int ASM_giWindowHeight = 600;
GLfloat ASM_angle;

GLXContext ASM_gGLXContext;

char ASM_key[26];

bool ASM_gbLight = false;

GLfloat ASM_lightAmbientZero[] = {0.0f,0.0f,0.0f,1.0f};
GLfloat ASM_lightDiffuseZero[] = { 1.0f,0.0f,0.0f,1.0f };
GLfloat ASM_lightSpecularZero[] = { 1.0f,0.0f,0.0f,1.0f };
GLfloat ASM_lightPositionZero[] = { 0.0f,0.0f,0.0f,1.0f }; 

GLfloat ASM_lightAmbientOne[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat ASM_lightDiffuseOne[] = { 0.0f,1.0f,0.0f,1.0f };
GLfloat ASM_lightSpecularOne[] = { 0.0f,1.0f,0.0f,1.0f };
GLfloat ASM_lightPositionOne[] = { 0.0f,0.0f,0.0f,1.0f };

GLfloat ASM_lightAmbientTwo[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat ASM_lightDiffuseTwo[] = { 0.0f,0.0f,1.0f,1.0f };
GLfloat ASM_lightSpecularTwo[] = { 0.0f,0.0f,1.0f,1.0f };
GLfloat ASM_lightPositionTwo[] = { 0.0f,0.0f,0.0f,1.0f };

GLfloat ASM_materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat ASM_materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat ASM_materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat ASM_materialShininess = 50.0f;

GLfloat ASM_lightAngle0 = 0.0f;
GLfloat ASM_lightAngle1 = 0.0f;
GLfloat ASM_lightAngle2 = 0.0f;

GLUquadric * ASM_quadric = NULL;

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
	int ASM_defaultScreen;
	int ASM_styleMask;
	
	static int ASM_FrameBufferAttributes[] = 
						{ GLX_DOUBLEBUFFER, True,
						  GLX_RGBA,
						  GLX_RED_SIZE, 8,
						  GLX_GREEN_SIZE, 8,
						  GLX_ALPHA_SIZE, 8,
						  GLX_DEPTH_SIZE, 24,
						  None						
						};
						
	ASM_gpDisplay = XOpenDisplay(NULL);
	
	if(ASM_gpDisplay == NULL)
	{
		printf("ERROR: Unable To Open X Display...Exit \n");
		Uninitialize();
		exit(1);
	}
	
	ASM_defaultScreen = XDefaultScreen(ASM_gpDisplay);
	
	ASM_gpVisualInfo = glXChooseVisual(ASM_gpDisplay, ASM_defaultScreen, ASM_FrameBufferAttributes);
	
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
		printf("ERROR: Failed to Create Main Window\n");
		Uninitialize();
		exit(1);	
	}
	
	XStoreName(ASM_gpDisplay, ASM_gWindow, "Sphere with Three Rotating Light");
	
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
	
	ASM_gGLXContext = glXCreateContext(ASM_gpDisplay, ASM_gpVisualInfo, NULL, GL_TRUE);
	
	glXMakeCurrent(ASM_gpDisplay, ASM_gWindow, ASM_gGLXContext);
	
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	////Light0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ASM_lightAmbientZero);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, ASM_lightDiffuseZero);
	glLightfv(GL_LIGHT0, GL_SPECULAR, ASM_lightSpecularZero);

	glEnable(GL_LIGHT0);

	////Light1
	glLightfv(GL_LIGHT1, GL_AMBIENT, ASM_lightAmbientOne);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, ASM_lightDiffuseOne);
	glLightfv(GL_LIGHT1, GL_SPECULAR, ASM_lightSpecularOne);

	glEnable(GL_LIGHT1);

	////Light2
	glLightfv(GL_LIGHT2, GL_AMBIENT, ASM_lightAmbientTwo);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, ASM_lightDiffuseTwo);
	glLightfv(GL_LIGHT2, GL_SPECULAR, ASM_lightSpecularTwo);

	glEnable(GL_LIGHT2);

	////Material
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);
	
	Resize(ASM_gWindowWidth, ASM_giWindowHeight);
	
	


}
void Resize(int width, int height){

	if(height <= 0)
		height = 1;
		
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f,100.0f);

}
void Render(){

	void Update();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glPushMatrix();
		
		gluLookAt(0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		
		glPushMatrix();
			glRotatef(ASM_lightAngle0, 1.0f, 0.0f, 0.0f);
			ASM_lightPositionZero[1] = ASM_lightAngle0;
			glLightfv(GL_LIGHT0, GL_POSITION, ASM_lightPositionZero);
		glPopMatrix();
		
		glPushMatrix();
			glRotatef(ASM_lightAngle1, 0.0f, 1.0f, 0.0f);
			ASM_lightPositionOne[0] = ASM_lightAngle1;
			glLightfv(GL_LIGHT1, GL_POSITION, ASM_lightPositionOne);
		glPopMatrix();
		
		glPushMatrix();
			glRotatef(ASM_lightAngle2, 0.0f, 0.0f, 1.0f);
			ASM_lightPositionTwo[0] = ASM_lightAngle2;
			glLightfv(GL_LIGHT2, GL_POSITION, ASM_lightPositionTwo);		
		glPopMatrix();
		
		glTranslatef(0.0f, 0.0f, -1.0f);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	
		ASM_quadric = gluNewQuadric();
		gluSphere(ASM_quadric, 0.8f, 50, 50);	
		
	glPopMatrix();
	
	Update();
	
	
	glXSwapBuffers(ASM_gpDisplay, ASM_gWindow);

}


void Update()
{

	ASM_lightAngle0 = ASM_lightAngle0 + 0.2f;
	if (ASM_lightAngle0 > 360.f)
	{
		ASM_lightAngle0 = 0.0f;
	}

	ASM_lightAngle1 = ASM_lightAngle1 + 0.2f;
	if (ASM_lightAngle1 > 360.f)
	{
		ASM_lightAngle1 = 0.0f;
	}

	ASM_lightAngle2 = ASM_lightAngle2 + 0.2f;
	if (ASM_lightAngle2 > 360.f)
	{
		ASM_lightAngle2 = 0.0f;
	}

}


void Uninitialize(){

	GLXContext ASM_currentGLXContext;
	ASM_currentGLXContext = glXGetCurrentContext();
	
	if (ASM_quadric)
	{
		gluDeleteQuadric(ASM_quadric);
		ASM_quadric = NULL;
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

}




























