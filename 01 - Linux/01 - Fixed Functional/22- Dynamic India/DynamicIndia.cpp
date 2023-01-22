#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include<math.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>


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

GLfloat ASM_ITranslate, ASM_I2Translate, ASM_NTranslate, ASM_ATranslate;
GLfloat ASM_ROrange, ASM_GOrange, ASM_BOrange;
GLfloat ASM_RWhite, ASM_GWhite, ASM_BWhite;
GLfloat ASM_RGreen, ASM_GGreen, ASM_BGreen;
GLfloat ASM_TAnglePlane1 = M_PI/2.0f, ASM_RAnglePlane1;
GLfloat ASM_TAnglePlane2 = M_PI / 2.0f;
GLfloat ASM_TPlane3;
GLfloat ASM_forward;
bool ASM_moveup = false;
GLfloat ASM_TAngleMoveUpPlane1 = M_PI, ASM_RAngleMoveUpPlane1;
GLfloat ASM_TAngleMoveUpPlane2;
GLfloat ASM_TMoveForwardPlane3;
GLfloat ASM_EndScene;

ALCcontext *ASM_context;
ALCdevice *ASM_device;
unsigned int ASM_alSource;      //source
unsigned int  ASM_alSampleSet;


GLXContext ASM_gGLXContext;


int main(void)
{
	void CreateWindow(void);
	void ToggleFullScreen(void);
	void Uninitialize(void);
	void Initialize(void);
	void Resize(int, int);
	void Render();
	void PlaySound();
	
	int ASM_winHeight = ASM_giWindowHeight;
	int ASM_winWidth = ASM_gWindowWidth;

	bool ASM_bDone = false;

	//code
	CreateWindow();
	Initialize();
	ToggleFullScreen();
	PlaySound();

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
	
	XStoreName(ASM_gpDisplay, ASM_gWIndow, "Dynamic India");
	
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

void PlaySound()
{
	void Uninitialize(void);

	char*     ASM_alBuffer;             //data for the buffer
	ALenum  ASM_alFormatBuffer;    	//buffer format
	ALsizei    ASM_alFreqBuffer;       	//frequency
	long        ASM_alBufferLen;       	//bit depth
	ALboolean     ASM_alLoop;         	//loop
	
	 
	 ASM_device = alcOpenDevice(NULL);
	
	if (ASM_device == NULL)
	{
	   Uninitialize();
	   exit(1);	
	}
	 
	//Create a context
	 ASM_context=alcCreateContext( ASM_device,NULL);
	 
	//Set active context
	alcMakeContextCurrent( ASM_context);
	 
	// Clear Error Code
	alGetError();
	 
	//load the wave file
	alutLoadWAVFile((ALbyte*)"Dulhan-Chali-Mahendra-Kapoor.wav",
			&ASM_alFormatBuffer, (void **)&ASM_alBuffer,
			(ALsizei*)&ASM_alBufferLen, 
			&ASM_alFreqBuffer, 
			&ASM_alLoop);
	
	//create a source
	alGenSources(1, &ASM_alSource);
	 
	//create  buffer
	alGenBuffers(1, &ASM_alSampleSet);
	 
	//put the data into our sampleset buffer
	alBufferData(ASM_alSampleSet, ASM_alFormatBuffer, ASM_alBuffer, ASM_alBufferLen, ASM_alFreqBuffer);
	 
	//assign the buffer to this source
	alSourcei(ASM_alSource, AL_BUFFER, ASM_alSampleSet);
	 
	//release the data
	alutUnloadWAV(ASM_alFormatBuffer, ASM_alBuffer, ASM_alBufferLen, ASM_alFreqBuffer);

	alSourcei(ASM_alSource,AL_LOOPING,AL_TRUE);
	 
	//play
	alSourcePlay(ASM_alSource);	 
	

	
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
	void DynamicIndia();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	DynamicIndia();	
	
	glXSwapBuffers(ASM_gpDisplay, ASM_gWIndow);

}


void DynamicIndia()
{
	//function declration
	void iIndia();
	void nIndia();
	void dIndia();
	void aIndia();
	void update();
	void Plane(GLfloat Red, GLfloat Green, GLfloat Blue);
	void FlagA();
	void Uninitialize();

	//variable declartion
	GLfloat ASM_z = -20.0f;
	
	
			//////////////////////////////////////// I
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glTranslatef(0.0f, 0.0f, ASM_z);
			glTranslatef(-5.0f, 0.0f, 0.0f);
			glTranslatef(-0.5f, 0.0f, 0.0f);
			glTranslatef(-10.5f, 0.0f, 0.0f);
			glTranslatef(ASM_ITranslate,0.0f,0.0f);

			glScalef(1.0f, 1.3f, 0.0f);
			iIndia();
			
			//////////////////////////////////////// N
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glTranslatef(0.0f, 0.0f, ASM_z);
			glTranslatef(-3.5f, 0.0f, 0.0f);
			
			glTranslatef(-0.5f, 0.0f, 0.0f);
			glTranslatef(0.0f, 10.5f, 0.0f);
			glTranslatef(0.0f, -ASM_NTranslate, 0.0f);

			glScalef(1.0f, 1.3f, 0.0f);
			nIndia();

			//////////////////////////////////////// D
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glTranslatef(0.0f, 0.0f, ASM_z);
			
			glTranslatef(-0.5f, 0.0f, 0.0f);
			glTranslatef(-0.5f, 0.0f, 0.0f);
			glScalef(1.0f, 1.3f, 0.0f);
			dIndia();

			//////////////////////////////////////// I
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glTranslatef(0.0f, 0.0f, ASM_z);
			
			glTranslatef(2.2f, 0.0f, 0.0f);
			glTranslatef(-0.5f, 0.0f, 0.0f);
			glTranslatef(0.0f, -10.5f, 0.0f);
			glTranslatef(0.0f, ASM_I2Translate, 0.0f);

			glScalef(1.0f, 1.3f, 0.0f);
			iIndia();

			//////////////////////////////////////// A
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glTranslatef(0.0f, 0.0f, ASM_z);
			glTranslatef(-0.5f, 0.0f, 0.0f);
			glTranslatef(3.2f, 0.0f, 0.0f);
			glTranslatef(13.5f, 0.0f, 0.0f);
			glTranslatef(-ASM_ATranslate, 0.0f, 0.0f);

			glScalef(1.0f, 1.3f, 0.0f);
			if (ASM_forward > 19.0f)
			{
				FlagA();
			}
			aIndia();	
			

			//////////////////////////////////////// Plane 1
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glTranslatef(0.0f, 0.0f, ASM_z);
			glScalef(0.6f, 0.6f, 0.0f);
			glTranslatef(-9.5f, 0.0f, 0.0f);
			glTranslatef(0.0f, 15.0f, 0.0f);
			

			glTranslatef(15*sin(-ASM_TAnglePlane1), 15*cos(-ASM_TAnglePlane1), 0.0f);
			glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
			glRotatef(ASM_RAnglePlane1, 0.0f, 0.0f, 1.0f);

			glTranslatef(ASM_forward, 0.0f, 0.0f);
			if (ASM_moveup == true)
			{
				glTranslatef(0.0f, 13.7f, 0.0f);

				glTranslatef(13.7 * sin(-ASM_TAngleMoveUpPlane1), 13.7 * cos(-ASM_TAngleMoveUpPlane1), 0.0f);
				glRotatef(ASM_RAngleMoveUpPlane1, 0.0f, 0.0f, 1.0f);
			}
			
			Plane(1.0f, 0.6f, 0.2f);


			//////////////////////////////////////////Plane 2
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glTranslatef(0.0f, 0.0f, ASM_z);
			glScalef(0.6f, 0.6f, 0.0f);
			glTranslatef(-9.5f, 0.0f, 0.0f);
			glTranslatef(0.0f, -15.0f, 0.0f);


			glTranslatef(-15 * sin(ASM_TAnglePlane2), -15 * cos(ASM_TAnglePlane2), 0.0f);
			glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
			glRotatef(-ASM_RAnglePlane1, 0.0f, 0.0f, 1.0f);
			glTranslatef(ASM_forward, 0.0f, 0.0f);

			if (ASM_moveup == true)
			{
				glTranslatef(0.0f, -13.7f, 0.0f);

				glTranslatef(13.7 * sin(ASM_TAngleMoveUpPlane2), 13.7 * cos(ASM_TAngleMoveUpPlane2), 0.0f);
				glRotatef(-ASM_RAngleMoveUpPlane1, 0.0f, 0.0f, 1.0f);
			}

			Plane(0.07f, 0.53f, 0.03f);

			//////////////////////////////////////////Plane 3
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glTranslatef(0.0f, 0.0f, ASM_z);
			glScalef(0.6f, 0.6f, 0.0f);
			glTranslatef(-9.5f, 0.0f, 0.0f);
			glTranslatef(-16.0f, 0.0f, 0.0f);

			glTranslatef(ASM_TPlane3, 0.0f, 0.0f);
			glTranslatef(ASM_forward, 0.0f, 0.0f);

			if (ASM_moveup == true)
			{
				glTranslatef(ASM_TMoveForwardPlane3, 0.0f, 0.0f);
				
			}
			Plane(1.0f, 1.0f, 1.0f);

			
	update();

	if(ASM_EndScene >= 0.9f)
	{
		Uninitialize();
		exit(0);
	}

	
}

void update()
{
	if (ASM_ITranslate < 10.5f)
	{
		ASM_ITranslate = ASM_ITranslate + (0.0105f / 2.0f);
	}
	else if (ASM_NTranslate < 10.5f)
	{
		ASM_NTranslate = ASM_NTranslate + (0.0105f / 2.0f);
	}
	else if (ASM_I2Translate < 10.5f)
	{
		ASM_I2Translate = ASM_I2Translate + (0.0155f / 2.0f);
	}
	else if (ASM_ATranslate < 12.5f)
	{
		ASM_ATranslate = ASM_ATranslate + (0.0135f / 2.0f);
	}
	else if (ASM_ROrange < 1.0f)
	{
		ASM_ROrange = ASM_ROrange + (0.001f / 2.0f);
		ASM_GOrange = ASM_GOrange + (0.0006 / 2.0f);
		ASM_BOrange = ASM_BOrange + (0.0002 / 2.0f);
		ASM_RWhite = ASM_RWhite + (0.001f / 2.0f);
		ASM_GWhite = ASM_GWhite + (0.001f / 2.0f);
		ASM_BWhite = ASM_BWhite + (0.001f / 2.0f);
		ASM_RGreen = ASM_RGreen + (0.00007 / 2.0f);
		ASM_GGreen = ASM_GGreen + (0.00053 / 2.0f);
		ASM_BGreen = ASM_BGreen + (0.00003 / 2.0f);

	}
	else if (ASM_RAnglePlane1 < 90.0f)
	{

		ASM_TAnglePlane1 = ASM_TAnglePlane1 + (0.00157f/2.0f);

		ASM_RAnglePlane1 = ASM_RAnglePlane1 + (0.09f / 2.0f);

		ASM_TAnglePlane2 = ASM_TAnglePlane2 + (0.00157f / 2.0f);

		ASM_TPlane3 = ASM_TPlane3 + (0.016 / 2.0f);
	}
	else if (ASM_forward < 22.0f)
	{
		ASM_forward = ASM_forward + (0.016 / 2.0f);
	}
	else if (ASM_RAngleMoveUpPlane1 < 90.0f)
	{
		ASM_TAngleMoveUpPlane1 = ASM_TAngleMoveUpPlane1 + (0.00157f / 2.0f);

		ASM_RAngleMoveUpPlane1 = ASM_RAngleMoveUpPlane1 + (0.09f / 2.0f);

		ASM_TAngleMoveUpPlane2 = ASM_TAngleMoveUpPlane2 + (0.00157f / 2.0f);

		ASM_TMoveForwardPlane3 = ASM_TMoveForwardPlane3 + (0.017 / 2.0f);

	}
	else if (ASM_EndScene < 1.0f)
	{
		ASM_EndScene = ASM_EndScene + 0.001f;
	}

	if (ASM_forward >= 22.0f)
		ASM_moveup = true;
}

void Plane(GLfloat Red, GLfloat Green, GLfloat Blue)
{
	//function declaration
	void FlagPlaneSingleColor(GLfloat Red, GLfloat Green, GLfloat Blue);
	void FlagPlaneTriColor();

	//front triangle
	glBegin(GL_TRIANGLES);
	glColor3f(0.71f, 0.89f, 0.95f);
		glVertex3f(0.7f, 0.0, 0.0f);
		glVertex3f(0.3f, 0.15f, 0.0f);
		glVertex3f(0.3f, -0.15f, 0.0f);
	glEnd();

	// front rectangle

	glBegin(GL_QUADS);

	glColor3f(0.71f, 0.89f, 0.95f);
	glVertex3f(0.3f, 0.15f, 0.0f);
	glVertex3f(0.3f, -0.15f, 0.0f);
	glVertex3f(-0.5f, -0.18, 0.0f);
	glVertex3f(-0.5f, 0.18, 0.0f);

	glEnd();

	//middle rectangle
	glBegin(GL_QUADS);

	glColor3f(0.71f, 0.89f, 0.95f);
	glVertex3f(-0.5f, 0.35, 0.0f);
	glVertex3f(-0.5f, -0.35, 0.0f);
	glVertex3f(-2.5f, -0.31, 0.0f);
	glVertex3f(-2.5f, 0.31, 0.0f);

	glEnd();


	// front upper wing
	glBegin(GL_QUADS);

	glColor3f(0.71f, 0.89f, 0.95f);
	glVertex3f(-0.6f, 0.18, 0.0f);
	glVertex3f(-2.0f, 0.18, 0.0f);
	glVertex3f(-3.0f, 1.5f, 0.0f);
	glVertex3f(-2.6f, 1.6f, 0.0f);

	glEnd();

	// front lower wing
	glBegin(GL_QUADS);

	glColor3f(0.71f, 0.89f, 0.95f);
	glVertex3f(-0.6f, -0.18, 0.0f);
	glVertex3f(-2.0f, -0.18, 0.0f);
	glVertex3f(-3.0f, -1.5f, 0.0f);
	glVertex3f(-2.6f, -1.6f, 0.0f);

	glEnd();

	// back upper wing
	glBegin(GL_POLYGON);

	glColor3f(0.71f, 0.89f, 0.95f);
	glVertex3f(-2.5f, 0.31, 0.0f);

	glVertex3f(-3.2f, 1.0f, 0.0f);
	glVertex3f(-3.4f, 1.0f, 0.0f);

	glVertex3f(-3.4f, -1.0f, 0.0f);
	glVertex3f(-3.2f, -1.0f, 0.0f);

	glVertex3f(-2.5f, -0.31, 0.0f);

	glEnd();

	glLineWidth(3.0f);
	//////////////////////////////////// I 
	// '|'
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-1.8f, 0.22f, 0.0f);
	glVertex3f(-1.8f, -0.22f, 0.0f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-1.6f, 0.22f, 0.0f);
	glVertex3f(-2.0f, 0.22f, 0.0f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-1.6f, -0.22f, 0.0f);
	glVertex3f(-2.0f, -0.22f, 0.0f);
	glEnd();

	//////////////////////////////////// A 
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-1.3f, 0.26f, 0.0f);
	glVertex3f(-1.5f, -0.26f, 0.0f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-1.3f, 0.26f, 0.0f);
	glVertex3f(-1.1f, -0.26f, 0.0f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-1.4f, 0.0f, 0.0f);
	glVertex3f(-1.2f, 0.0f, 0.0f);
	glEnd();

	//////////////////////////////////// F

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-0.9f, 0.22f, 0.0f);
	glVertex3f(-0.9f, -0.26f, 0.0f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, 0.22f, 0.0f);
	glVertex3f(-0.6f, 0.22, 0.0f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-0.6f, 0.0f, 0.0f);
	glEnd();


	//////////////////////////////////// Smoke
	glTranslatef(-3.9f, 0.0f, 0.0f);

	if (ASM_TPlane3 < 15.9f || ASM_forward >= 22.0f)  
	{
		FlagPlaneSingleColor(Red, Green, Blue);
		
	}
	else
	{
		FlagPlaneTriColor();
	}
}

void FlagPlaneSingleColor(GLfloat Red, GLfloat Green, GLfloat Blue)
{
	// Flag strip

	glBegin(GL_QUADS);

	glColor3f(Red, Green, Blue);
	glVertex3f(0.5f, 0.15f * 2.5f, 0.0f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-1.2f, 0.15f * 2.5f, 0.0f);
	glVertex3f(-1.2f, -0.15f * 2.5f, 0.0f);

	glColor3f(Red, Green, Blue);
	glVertex3f(0.5f, -0.15f * 2.5f, 0.0f);

	glEnd();
}

void FlagPlaneTriColor()
{
	// Saffron strip
	

	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex3f(0.5f, 0.15f * 2.5f, 0.0f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-1.2f, 0.15f * 2.5f, 0.0f);
	glVertex3f(-1.2f, 0.05f * 2.5f, 0.0f);

	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex3f(0.5f, 0.05f * 2.5f, 0.0f);

	glEnd();

	// White strip

	glBegin(GL_QUADS);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, 0.05f * 2.5f, 0.0f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-1.2f, 0.05f * 2.5f, 0.0f);
	glVertex3f(-1.2f, -0.05f * 2.5f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, -0.05f * 2.5f, 0.0f);

	glEnd();

	// Green strip

	glBegin(GL_QUADS);

	glColor3f(0.07f, 0.53f, 0.03f);
	glVertex3f(0.5f, -0.05f * 2.5f, 0.0f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-1.2f, -0.05f * 2.5f, 0.0f);
	glVertex3f(-1.2f, -0.15f * 2.5f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);
	glVertex3f(0.5f, -0.15f * 2.5f, 0.0f);

	glEnd();
}


void iIndia()
{
	// '-' top
	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex3f(0.8f, 1.4f, 0.0f);
	glVertex3f(-0.8f, 1.4f, 0.0f);
	glVertex3f(-0.8f, 1.0f, 0.0f);
	glVertex3f(0.8f, 1.0f, 0.0f);

	glEnd();

	//  '|'
	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex3f(0.2f, 1.0f, 0.0f);

	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex3f(-0.2f, 1.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.2f, 0.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.2f, 0.0f, 0.0f);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.2f, 0.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.2f, 0.0f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);
	glVertex3f(-0.2f, -1.0f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);
	glVertex3f(0.2f, -1.0f, 0.0f);

	glEnd();

	//////  bottom '-'

	glBegin(GL_QUADS);

	glColor3f(0.07f, 0.53f, 0.03f);
	glVertex3f(0.8f, -1.4f, 0.0f);
	glVertex3f(-0.8f, -1.4f, 0.0f);
	glVertex3f(-0.8f, -1.0f, 0.0f);
	glVertex3f(0.8f, -1.0f, 0.0f);

	glEnd();

}

void nIndia()
{

	// Left |
	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex3f(0.2f, 1.4f, 0.0f);

	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex3f(-0.2f, 1.4f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.2f, 0.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.2f, -0.0f, 0.0f);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.2f, 0.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.2f, 0.0f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);
	glVertex3f(-0.2f, -1.4f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);
	glVertex3f(0.2f, -1.4f, 0.0f);

	glEnd();

	//     | right
	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex3f(1.8f, 1.4f, 0.0f);

	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex3f(1.4f, 1.4f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.4f, 0.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.8f, -0.0f, 0.0f);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.8f, 0.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.4f, 0.0f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);
	glVertex3f(1.4f, -1.4f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);
	glVertex3f(1.8f, -1.4f, 0.0f);

	glEnd();

	///////   '\'

	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex3f(0.2f, 1.4f, 0.0f);

	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex3f(-0.2f, 1.4f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.6f, 0.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);



	glEnd();

	glBegin(GL_QUADS);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.6f, 0.0f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);
	glVertex3f(1.4f, -1.4f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);
	glVertex3f(1.8f, -1.4f, 0.0f);

	glEnd();

}

void dIndia()
{

	// Left |
	glBegin(GL_QUADS);

	

	glColor3f(ASM_ROrange, ASM_GOrange, ASM_BOrange);
	glVertex3f(0.2f, 1.4f, 0.0f);

	glColor3f(ASM_ROrange, ASM_GOrange, ASM_BOrange);
	glVertex3f(-0.2f, 1.4f, 0.0f);

	glColor3f(ASM_RWhite, ASM_GWhite, ASM_BWhite);
	glVertex3f(-0.2f, 0.0f, 0.0f);

	glColor3f(ASM_RWhite, ASM_GWhite, ASM_BWhite);
	glVertex3f(0.2f, -0.0f, 0.0f);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(ASM_RWhite, ASM_GWhite, ASM_BWhite);
	glVertex3f(0.2f, 0.0f, 0.0f);

	glColor3f(ASM_RWhite, ASM_GWhite, ASM_BWhite);
	glVertex3f(-0.2f, 0.0f, 0.0f);

	glColor3f(ASM_RGreen, ASM_GGreen, ASM_BGreen);
	glVertex3f(-0.2f, -1.4f, 0.0f);

	glColor3f(ASM_RGreen, ASM_GGreen, ASM_BGreen);
	glVertex3f(0.2f, -1.4f, 0.0f);

	glEnd();

	//////  top '-'
	glBegin(GL_QUADS);

	glColor3f(ASM_ROrange, ASM_GOrange, ASM_BOrange);
	glVertex3f(1.4f, 1.4f, 0.0f);
	glVertex3f(-0.6f, 1.4f, 0.0f);
	glVertex3f(-0.6f, 1.0f, 0.0f);
	glVertex3f(1.4f, 1.0f, 0.0f);

	glEnd();


	//////  bottom '-'
	glBegin(GL_QUADS);

	glColor3f(ASM_RGreen, ASM_GGreen, ASM_BGreen);
	glVertex3f(1.4f, -1.4f, 0.0f);
	glVertex3f(-0.6f, -1.4f, 0.0f);
	glVertex3f(-0.6f, -1.0f, 0.0f);
	glVertex3f(1.4f, -1.0f, 0.0f);

	glEnd();

	// right |
	glBegin(GL_QUADS);

	glColor3f(ASM_ROrange, ASM_GOrange, ASM_BOrange);
	glVertex3f(1.4f, 1.4f, 0.0f);

	glColor3f(ASM_ROrange, ASM_GOrange, ASM_BOrange);
	glVertex3f(1.0f, 1.4f, 0.0f);

	glColor3f(ASM_RWhite, ASM_GWhite, ASM_BWhite);
	glVertex3f(1.0f, 0.0f, 0.0f);

	glColor3f(ASM_RWhite, ASM_GWhite, ASM_BWhite);
	glVertex3f(1.4f, -0.0f, 0.0f);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(ASM_RWhite, ASM_GWhite, ASM_BWhite);
	glVertex3f(1.0f, 0.0f, 0.0f);

	glColor3f(ASM_RWhite, ASM_GWhite, ASM_BWhite);
	glVertex3f(1.4f, -0.0f, 0.0f);

	glColor3f(ASM_RGreen, ASM_GGreen, ASM_BGreen);
	glVertex3f(1.4f, -1.4f, 0.0f);

	glColor3f(ASM_RGreen, ASM_GGreen, ASM_BGreen);
	glVertex3f(1.0f, -1.4f, 0.0f);

	glEnd();

}

void aIndia()
{
	
	// '/' left
	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex3f(0.2f, 1.4f, 0.0f);

	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex3f(-0.2f, 1.4f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.6f, 0.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.2f, 0.0f, 0.0f);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.2f, 0.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.6f, 0.0f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);
	glVertex3f(-1.0f, -1.4f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);
	glVertex3f(-0.6f, -1.4f, 0.0f);

	glEnd();

	// '\' right
	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex3f(0.2f, 1.4f, 0.0f);

	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex3f(-0.2f, 1.4f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.2f, 0.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.6f, 0.0f, 0.0f);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.2f, 0.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.6f, 0.0f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);
	glVertex3f(1.0f, -1.4f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);
	glVertex3f(0.6f, -1.4f, 0.0f);

	glEnd();
}



void FlagA()
{
	// Saffron strip

	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.6f, 0.2f);
	glVertex3f(0.5f, 0.15f, 0.0f);
	glVertex3f(-0.5f, 0.15f , 0.0f);
	glVertex3f(-0.5f, 0.05f , 0.0f);
	glVertex3f(0.5f, 0.05f , 0.0f);

	glEnd();

	// White strip

	glBegin(GL_QUADS);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, 0.05f , 0.0f);
	glVertex3f(-0.5f, 0.05f , 0.0f);
	glVertex3f(-0.5f, -0.05f , 0.0f);
	glVertex3f(0.5f, -0.05f , 0.0f);

	glEnd();

	// Green strip

	glBegin(GL_QUADS);

	glColor3f(0.07f, 0.53f, 0.03f);
	glVertex3f(0.5f, -0.05f , 0.0f);
	glVertex3f(-0.5f, -0.05f , 0.0f);
	glVertex3f(-0.5f, -0.15f , 0.0f);
	glVertex3f(0.5f, -0.15f , 0.0f);

	glEnd();
}

void Uninitialize(void)
{
	GLXContext ASM_currentGLXCOntext;
	ASM_currentGLXCOntext = glXGetCurrentContext();
	
	//---------------------------------------------Sound
	alDeleteSources(1,&ASM_alSource);
	 
	//delete our buffer
	alDeleteBuffers(1,&ASM_alSampleSet);
	 
	ASM_context=alcGetCurrentContext();
	 
	//Get device for active context
	ASM_device=alcGetContextsDevice(ASM_context);
	 
	//Disable context
	alcMakeContextCurrent(NULL);
	 
	//Release context(s)
	alcDestroyContext(ASM_context);
	 
	//Close device
	alcCloseDevice(ASM_device);
	
	//-------------------------------------------------
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

