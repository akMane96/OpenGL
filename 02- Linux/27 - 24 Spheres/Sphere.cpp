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

GLfloat ASM_light_model_ambient[] = { 0.2f,0.2f,0.2f,1.0f };
GLfloat ASM_light_model_local_viewer[] = {0.0f};

GLfloat ASM_lightAngleX = 0.0f;
GLfloat ASM_lightAngleY = 0.0f;
GLfloat ASM_lightAngleZ = 0.0f;

int ASM_KeyPressed = 0;

GLfloat ASM_lightAmbient[] = {0.0f,0.0f,0.0f,1.0f};
GLfloat ASM_lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat ASM_lightPosition[] = { 0.0f,3.0f,3.0f,0.0f };  //// Directional Light

GLUquadric* ASM_quadric[24];

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
							
						case 'x':
						case 'X':
							ASM_KeyPressed = 1;
							ASM_lightAngleX = 0.0f;				
							break;

						case 'y':
						case 'Y':
							ASM_KeyPressed = 2;
							ASM_lightAngleY = 0.0f;
							break;

						case 'z':
						case 'Z':
							ASM_KeyPressed = 3;
							ASM_lightAngleZ = 0.0f;				
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
	
	XStoreName(ASM_gpDisplay, ASM_gWindow, "24 Spheres");
	
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
	
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
	
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ASM_light_model_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, ASM_light_model_local_viewer);

	////Light0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ASM_lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, ASM_lightDiffuse);
	

	glEnable(GL_LIGHT0);

	for (int ASM_i = 0; ASM_i < 24; ASM_i++)
	{
		ASM_quadric[ASM_i] = gluNewQuadric();
	}
	
	Resize(ASM_gWindowWidth, ASM_giWindowHeight);
	
	


}
void Resize(int width, int height){

	if(height <= 0)
		height = 1;
		
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	if (height > width)
	{
		glOrtho(0.0f, 15.5f, 0.0f, 15.5f * ((GLfloat)height / (GLfloat)width), -10.0f, 10.0f);
	}else
	{
		glOrtho(0.0f, 15.5f * ((GLfloat)width/ (GLfloat)height), 0.0f, 15.5f , -10.0f, 10.0f);
	}

}
void Render(){

	void Update();
	void Draw24Spheres(void);

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (ASM_KeyPressed == 1)
	{
		glRotatef(ASM_lightAngleX, 1.0f, 0.0f, 0.0f);
		ASM_lightPosition[1] = ASM_lightAngleX;

	}
	else if (ASM_KeyPressed == 2)
	{
		glRotatef(ASM_lightAngleY, 0.0f, 1.0f, 0.0f);
		ASM_lightPosition[2] = ASM_lightAngleY;

	}
	else if (ASM_KeyPressed == 3)
	{
		glRotatef(ASM_lightAngleZ, 0.0f, 0.0f, 1.0f);
		ASM_lightPosition[0] = ASM_lightAngleZ;

	}

	glLightfv(GL_LIGHT0, GL_POSITION, ASM_lightPosition);
	Draw24Spheres();
		
	Update();
	
	
	glXSwapBuffers(ASM_gpDisplay, ASM_gWindow);

}

void Draw24Spheres()
{

	GLfloat ASM_materialAmbient[4];
	GLfloat ASM_materialDiffuse[4] ;
	GLfloat ASM_materialSpecular[4];
	GLfloat ASM_materialShininess;

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//////////////////////////////////////////////////////***** 1st sphere on 1st column, emerald *****
	ASM_materialAmbient[0] = 0.0215;
	ASM_materialAmbient[1] = 0.1745;
	ASM_materialAmbient[2] = 0.0215;
	ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.07568;
	ASM_materialDiffuse[1] = 0.61424;
	ASM_materialDiffuse[2] = 0.07568;
	ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.633;
	ASM_materialSpecular[1] = 0.727811;
	ASM_materialSpecular[2] = 0.633;
	ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.6 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.5f,14.0f, 0.0f);

	gluSphere(ASM_quadric[0], 1.0f, 30, 30);

	////////////////////////////////////////////////////// ***** 2nd sphere on 1st column, jade *****
	ASM_materialAmbient[0] = 0.135;
	ASM_materialAmbient[1] = 0.2225;
	ASM_materialAmbient[2] = 0.1575;
	ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.54;
	ASM_materialDiffuse[1] = 0.89;
	ASM_materialDiffuse[2] = 0.63;
	ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.316228;
	ASM_materialSpecular[1] = 0.316228;
	ASM_materialSpecular[2] = 0.316228;
	ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.1 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.5f, 11.5f, 0.0f);

	gluSphere(ASM_quadric[1], 1.0f, 30, 30);

	////////////////////////////////////////////////////// ***** 3rd sphere on 1st column, obsidian *****
	ASM_materialAmbient[0] = 0.05375;
	ASM_materialAmbient[1] = 0.05;
	ASM_materialAmbient[2] = 0.06625;
	ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.18275;
	ASM_materialDiffuse[1] = 0.17;
	ASM_materialDiffuse[2] = 0.22525;
	ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.332741;
	ASM_materialSpecular[1] = 0.328634;
	ASM_materialSpecular[2] = 0.346435;
	ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.3 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.5f, 9.0f, 0.0f);

	gluSphere(ASM_quadric[2], 1.0f, 30, 30);

	////////////////////////////////////////////////////// ***** 4th sphere on 1st column, pearl *****
	ASM_materialAmbient[0] = 0.25;
	ASM_materialAmbient[1] = 0.20725;
	ASM_materialAmbient[2] = 0.20725;
	ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 1.0;
	ASM_materialDiffuse[1] = 0.829;
	ASM_materialDiffuse[2] = 0.829;
	ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.296648;
	ASM_materialSpecular[1] = 0.296648;
	ASM_materialSpecular[2] = 0.296648;
	ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.088 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.5f, 6.5f, 0.0f);

	gluSphere(ASM_quadric[3], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 5th sphere on 1st column, ruby *****
	ASM_materialAmbient[0] = 0.1745;
	ASM_materialAmbient[1] = 0.01175;
	ASM_materialAmbient[2] = 0.01175;
	ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.61424;
	ASM_materialDiffuse[1] = 0.04136;
	ASM_materialDiffuse[2] = 0.04136;
	ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.727811;
	ASM_materialSpecular[1] = 0.626959;
	ASM_materialSpecular[2] = 0.626959;
	ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.6 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.5f, 4.0f, 0.0f);

	gluSphere(ASM_quadric[4], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 6th sphere on 1st column, turquoise *****
	ASM_materialAmbient[0] = 0.1;
	ASM_materialAmbient[1] = 0.18725;
	ASM_materialAmbient[2] = 0.1745;
	ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.396;
	ASM_materialDiffuse[1] = 0.74151;
	ASM_materialDiffuse[2] = 0.69102;
	ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.297254;
	ASM_materialSpecular[1] = 0.30829;
	ASM_materialSpecular[2] = 0.306678;
	ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.1 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.5f, 1.5f, 0.0f);

	gluSphere(ASM_quadric[5], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 1st sphere on 2nd column, brass *****
	ASM_materialAmbient[0] = 0.329412;
		ASM_materialAmbient[1] = 0.223529;
		ASM_materialAmbient[2] = 0.027451;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.780392;
		ASM_materialDiffuse[1] = 0.568627;
		ASM_materialDiffuse[2] = 0.113725;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.992157;
		ASM_materialSpecular[1] = 0.941176;
		ASM_materialSpecular[2] = 0.807843;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.21794872 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(9.0f, 14.0f, 0.0f);

	gluSphere(ASM_quadric[6], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 2nd sphere on 2nd column, bronze *****
	ASM_materialAmbient[0] = 0.2125;
		ASM_materialAmbient[1] = 0.1275;
		ASM_materialAmbient[2] = 0.054;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.714;
		ASM_materialDiffuse[1] = 0.4284;
		ASM_materialDiffuse[2] = 0.18144;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.393548;
		ASM_materialSpecular[1] = 0.271906;
		ASM_materialSpecular[2] = 0.166721;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.2 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(9.0f, 11.5f, 0.0f);

	gluSphere(ASM_quadric[7], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 3rd sphere on 2nd column, chrome *****
	ASM_materialAmbient[0] = 0.25;
		ASM_materialAmbient[1] = 0.25;
		ASM_materialAmbient[2] = 0.25;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.4;
		ASM_materialDiffuse[1] = 0.4;
		ASM_materialDiffuse[2] = 0.4;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.774597;
		ASM_materialSpecular[1] = 0.774597;
		ASM_materialSpecular[2] = 0.774597;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.6 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(9.0f, 9.0f, 0.0f);

	gluSphere(ASM_quadric[8], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 4th sphere on 2nd column, copper *****
	ASM_materialAmbient[0] = 0.19125;
		ASM_materialAmbient[1] = 0.0735;
		ASM_materialAmbient[2] = 0.0225;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.7038;
		ASM_materialDiffuse[1] = 0.27048;
		ASM_materialDiffuse[2] = 0.0828;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.256777;
		ASM_materialSpecular[1] = 0.137622;
		ASM_materialSpecular[2] = 0.086014;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.1 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(9.0f, 6.5f, 0.0f);

	gluSphere(ASM_quadric[9], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 5th sphere on 2nd column, gold *****
	ASM_materialAmbient[0] = 0.24725;
		ASM_materialAmbient[1] = 0.1995;
		ASM_materialAmbient[2] = 0.0745;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.75164;
		ASM_materialDiffuse[1] = 0.60648;
		ASM_materialDiffuse[2] = 0.22648;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.628281;
		ASM_materialSpecular[1] = 0.555802;
		ASM_materialSpecular[2] = 0.366065;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.4 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(9.0f, 4.0f, 0.0f);

	gluSphere(ASM_quadric[10], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 6th sphere on 2nd column, silver *****
	ASM_materialAmbient[0] = 0.19225;
		ASM_materialAmbient[1] = 0.19225;
		ASM_materialAmbient[2] = 0.19225;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.50754;
		ASM_materialDiffuse[1] = 0.50754;
		ASM_materialDiffuse[2] = 0.50754;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.508273;
		ASM_materialSpecular[1] = 0.508273;
		ASM_materialSpecular[2] = 0.508273;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.4 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(9.0f, 1.5f, 0.0f);

	gluSphere(ASM_quadric[11], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 1st sphere on 3rd column, black *****
	ASM_materialAmbient[0] = 0.0;
		ASM_materialAmbient[1] = 0.0;
		ASM_materialAmbient[2] = 0.0;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.01;
		ASM_materialDiffuse[1] = 0.01;
		ASM_materialDiffuse[2] = 0.01;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.50;
		ASM_materialSpecular[1] = 0.50;
		ASM_materialSpecular[2] = 0.50;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.25 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(18.0f, 14.0f, 0.0f);

	gluSphere(ASM_quadric[12], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 2nd sphere on 3rd column, cyan *****
	ASM_materialAmbient[0] = 0.0;
		ASM_materialAmbient[1] = 0.1;
		ASM_materialAmbient[2] = 0.06;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.0;
		ASM_materialDiffuse[1] = 0.50980392;
		ASM_materialDiffuse[2] = 0.50980392;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.50196078;
		ASM_materialSpecular[1] = 0.50196078;
		ASM_materialSpecular[2] = 0.50196078;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.25 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(18.0f, 11.5f, 0.0f);

	gluSphere(ASM_quadric[13], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 3rd sphere on 2nd column, green *****
	ASM_materialAmbient[0] = 0.0;
		ASM_materialAmbient[1] = 0.0;
		ASM_materialAmbient[2] = 0.0;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.1;
		ASM_materialDiffuse[1] = 0.35;
		ASM_materialDiffuse[2] = 0.1;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.45;
		ASM_materialSpecular[1] = 0.55;
		ASM_materialSpecular[2] = 0.45;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.25 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(18.0f, 9.0f, 0.0f);

	gluSphere(ASM_quadric[14], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 4th sphere on 3rd column, red *****
	ASM_materialAmbient[0] = 0.0;
		ASM_materialAmbient[1] = 0.0;
		ASM_materialAmbient[2] = 0.0;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.5;
		ASM_materialDiffuse[1] = 0.0;
		ASM_materialDiffuse[2] = 0.0;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.7;
		ASM_materialSpecular[1] = 0.6;
		ASM_materialSpecular[2] = 0.6;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.25 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(18.0f, 6.5f, 0.0f);

	gluSphere(ASM_quadric[15], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 5th sphere on 3rd column, white ****
	ASM_materialAmbient[0] = 0.0;
		ASM_materialAmbient[1] = 0.0;
		ASM_materialAmbient[2] = 0.0;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.55;
		ASM_materialDiffuse[1] = 0.55;
		ASM_materialDiffuse[2] = 0.55;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.70;
		ASM_materialSpecular[1] = 0.70;
		ASM_materialSpecular[2] = 0.70;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.25 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(18.0f, 4.0f, 0.0f);

	gluSphere(ASM_quadric[16], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 6th sphere on 3rd column, yellow plastic *****
	ASM_materialAmbient[0] = 0.0;
		ASM_materialAmbient[1] = 0.0;
		ASM_materialAmbient[2] = 0.0;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.5;
		ASM_materialDiffuse[1] = 0.5;
		ASM_materialDiffuse[2] = 0.0;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.60;
		ASM_materialSpecular[1] = 0.60;
		ASM_materialSpecular[2] = 0.50;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.25 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(18.0f, 1.5f, 0.0f);

	gluSphere(ASM_quadric[17], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 1st sphere on 4th column, black *****
	ASM_materialAmbient[0] = 0.02;
		ASM_materialAmbient[1] = 0.02;
		ASM_materialAmbient[2] = 0.02;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.01;
		ASM_materialDiffuse[1] = 0.01;
		ASM_materialDiffuse[2] = 0.01;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.4;
		ASM_materialSpecular[1] = 0.4;
		ASM_materialSpecular[2] = 0.4;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.078125 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(26.0f, 14.0f, 0.0f);

	gluSphere(ASM_quadric[18], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 2nd sphere on 4th column, cyan *****
	ASM_materialAmbient[0] = 0.0;
		ASM_materialAmbient[1] = 0.05;
		ASM_materialAmbient[2] = 0.05;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.4;
		ASM_materialDiffuse[1] = 0.5;
		ASM_materialDiffuse[2] = 0.5;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.04;
		ASM_materialSpecular[1] = 0.7;
		ASM_materialSpecular[2] = 0.7;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.078125 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(26.0f, 11.5f, 0.0f);

	gluSphere(ASM_quadric[19], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 3rd sphere on 4th column, green *****
	ASM_materialAmbient[0] = 0.0;
		ASM_materialAmbient[1] = 0.05;
		ASM_materialAmbient[2] = 0.0;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.4;
		ASM_materialDiffuse[1] = 0.5;
		ASM_materialDiffuse[2] = 0.4;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.04;
		ASM_materialSpecular[1] = 0.7;
		ASM_materialSpecular[2] = 0.04;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.078125 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(26.0f, 9.0f, 0.0f);

	gluSphere(ASM_quadric[20], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 4th sphere on 4th column, red *****
	ASM_materialAmbient[0] = 0.05;
		ASM_materialAmbient[1] = 0.0;
		ASM_materialAmbient[2] = 0.0;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.5;
		ASM_materialDiffuse[1] = 0.4;
		ASM_materialDiffuse[2] = 0.4;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.7;
		ASM_materialSpecular[1] = 0.04;
		ASM_materialSpecular[2] = 0.04;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.078125 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(26.0f, 6.5f, 0.0f);

	gluSphere(ASM_quadric[21], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 5th sphere on 4th column, white *****
	ASM_materialAmbient[0] = 0.05;
		ASM_materialAmbient[1] = 0.05;
		ASM_materialAmbient[2] = 0.05;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.5;
		ASM_materialDiffuse[1] = 0.5;
		ASM_materialDiffuse[2] = 0.5;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.7;
		ASM_materialSpecular[1] = 0.7;
		ASM_materialSpecular[2] = 0.7;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.078125 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(26.0f, 4.0f, 0.0f);

	gluSphere(ASM_quadric[22], 1.0f, 30, 30);

	//////////////////////////////////////////////////////***** 6th sphere on 4th column, yellow rubber *****
	ASM_materialAmbient[0] = 0.05;
		ASM_materialAmbient[1] = 0.05;
		ASM_materialAmbient[2] = 0.0;
		ASM_materialAmbient[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ASM_materialAmbient);

	ASM_materialDiffuse[0] = 0.5;
		ASM_materialDiffuse[1] = 0.5;
		ASM_materialDiffuse[2] = 0.4;
		ASM_materialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ASM_materialDiffuse);

	ASM_materialSpecular[0] = 0.7;
		ASM_materialSpecular[1] = 0.7;
		ASM_materialSpecular[2] = 0.04;
		ASM_materialSpecular[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, ASM_materialSpecular);

	ASM_materialShininess = 0.078125 * 128;
		glMaterialf(GL_FRONT, GL_SHININESS, ASM_materialShininess);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(26.0f, 1.5f, 0.0f);

	gluSphere(ASM_quadric[23], 1.0f, 30, 30);

}



void Update()
{

	if (ASM_KeyPressed == 1)
	{
		ASM_lightAngleX = ASM_lightAngleX + 1.0f;

	}
	else if (ASM_KeyPressed == 2)
	{
		ASM_lightAngleY = ASM_lightAngleY + 1.0f;

	}
	else if (ASM_KeyPressed == 3)
	{
		ASM_lightAngleZ = ASM_lightAngleZ + 1.0f;

	}

}


void Uninitialize(){

	GLXContext ASM_currentGLXContext;
	ASM_currentGLXContext = glXGetCurrentContext();
	
	for (int ASM_i = 0; ASM_i < 24; ASM_i++)
	{
		if (ASM_quadric[ASM_i])
		{
			gluDeleteQuadric(ASM_quadric[ASM_i]);
			ASM_quadric[ASM_i] = NULL;
		}
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




























