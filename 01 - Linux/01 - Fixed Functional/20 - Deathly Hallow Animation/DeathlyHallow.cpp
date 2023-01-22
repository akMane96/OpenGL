#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>


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

GLfloat ASM_RAngleTriangle, ASM_RAngleCircle;
GLfloat tXTriangle, tYTriangle;
GLfloat tXCircle, tYCircle;
GLfloat tYLine;


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
	
	XStoreName(ASM_gpDisplay, ASM_gWIndow, "Deathly Hallow");
	
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
	
	ASM_gGLXContext = glXCreateContext(ASM_gpDisplay, ASM_gpVisualInfo, NULL, GL_TRUE);
	
	glXMakeCurrent(ASM_gpDisplay, ASM_gWIndow, ASM_gGLXContext);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
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

void Render()
{
	//funation declration
	void DeathlyHallows();
	
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -5.0f);	

	DeathlyHallows();
	
	glXSwapBuffers(ASM_gpDisplay, ASM_gWIndow);

}



void DeathlyHallows()
{
	//function prtotype
	void update();

	//variable declaration
	GLfloat ASM_angle, ASM_part;
	GLfloat ASM_RaidusIncircle;
	GLfloat ASM_move;
		

	//Assignment
	ASM_RaidusIncircle = 0.0f;
	
	//Calculations
	ASM_part = sqrtf(3.0f) / 3.0f;
	ASM_RaidusIncircle = ASM_part;

	// Triangle Co-dordinate
	GLfloat ASM_XCo[3] = { 0.0f, -1.0f, 1.0f };
	GLfloat ASM_YCo[3] = { 2*ASM_part, -ASM_part, -ASM_part };

	/////////////////////////////////////////////////////////////////    Triangle
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(0.0f, 0.0f, -8.0f);	

	glTranslatef(-5.0f, -5.0f, 0.0f);

	glTranslatef(tXTriangle, tYTriangle, 0.0f);

	glRotatef(ASM_RAngleTriangle, 0.0f, 1.0f, 0.0f);
	
	glLineWidth(4.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(ASM_XCo[0], ASM_YCo[0], 0.0f);
	glVertex3f(ASM_XCo[1], ASM_YCo[1], 0.0f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(ASM_XCo[1], ASM_YCo[1], 0.0f);
	glVertex3f(ASM_XCo[2], ASM_YCo[2], 0.0f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(ASM_XCo[2], ASM_YCo[2], 0.0f);
	glVertex3f(ASM_XCo[0], ASM_YCo[0], 0.0f);
	glEnd();	

	


	///////////////////////////////////////////////////////////////////    InCircle
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	
	glTranslatef(0.0f, 0.0f, -8.0f);

	glTranslatef(5.0f, -5.0f, 0.0f);

	glTranslatef(-tXCircle , tYCircle, 0.0f);

	glRotatef(ASM_RAngleCircle, 0.0f, 1.0f, 0.0f);

	glPointSize(4.0f);
	//Circle
	for (ASM_angle = 0.0f; ASM_angle <= (2 * M_PI); (ASM_angle = ASM_angle + 0.0001f))
	{
		glBegin(GL_POINTS);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(ASM_RaidusIncircle *sin(ASM_angle), ASM_RaidusIncircle *cos(ASM_angle), 0.0f);
		glEnd();
	}

	/////////////////////////////////////////////////////////////////////    Line
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -8.0f);

	glTranslatef(0.0f, sqrtf(50), 0.0f);

	glTranslatef(0.0f, -tYLine, 0.0f);

	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 2 * ASM_part, 0.0f);
	glVertex3f(0.0f, -ASM_part, 0.0f);
	glEnd();


	update();


}

void update()
{
	//Shapes Translate
	if (tXTriangle < 5.0f && tXTriangle < 5.0f)
	{
		tXTriangle = tXTriangle + 0.01f;
		tYTriangle = tYTriangle + 0.01f;
		ASM_RAngleTriangle = ASM_RAngleTriangle + 2.5f;
	}
	else if (tXCircle < 5.0f && tXCircle < 5.0f)
	{
		ASM_RAngleTriangle = 0.0f;
		tXCircle = tXCircle + 0.01f;
		tYCircle = tYCircle + 0.01f;
		ASM_RAngleCircle = ASM_RAngleCircle + 2.5f;
	}
	else if (tYLine < sqrt(50))
	{
		ASM_RAngleCircle = 0.0f;
		tYLine = tYLine + 0.03f;
	}

	
	if (ASM_RAngleTriangle > 360.0f)
	{
		ASM_RAngleTriangle = 0.0f;
	}

	if (ASM_RAngleCircle > 360.0f)
	{
		ASM_RAngleCircle = 0.0f;
	}


}
void Uninitialize(void)
{
	GLXContext ASM_currentGLXCOntext;
	ASM_currentGLXCOntext = glXGetCurrentContext();
	
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

