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

char ASM_keys[26];

GLXContext ASM_gGLXContext;

GLuint ASM_texture_smiley;
int ASM_keyPressed = 0;


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
							break;
							
										
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
					
					XLookupString(&ASM_event.xkey, ASM_keys, sizeof(ASM_keys), NULL, NULL);
					switch(ASM_keys[0])
					{
					
						case '1':
							ASM_keyPressed = 1;
							glEnable(GL_TEXTURE_2D);
							break;
							
						case '2':
							ASM_keyPressed = 2;
							glEnable(GL_TEXTURE_2D);
							break;
							
						case '3':
							ASM_keyPressed = 3;
							glEnable(GL_TEXTURE_2D);
							break;
							
						case '4':
							ASM_keyPressed = 4;
							glEnable(GL_TEXTURE_2D);
							break;
							
						default:
							glDisable(GL_TEXTURE_2D);
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
					break;
					
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
	
	XStoreName(ASM_gpDisplay, ASM_gWIndow, "Akshay XWIndow Tweaked Smiley");
	
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
	GLuint loadBitmapAsTexture(const char* );
	
	ASM_gGLXContext = glXCreateContext(ASM_gpDisplay, ASM_gpVisualInfo, NULL, GL_TRUE);
	
	glXMakeCurrent(ASM_gpDisplay, ASM_gWIndow, ASM_gGLXContext);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	ASM_texture_smiley = loadBitmapAsTexture("Smiley.bmp");
	
	Resize(ASM_gWindowWidth, ASM_giWindowHeight);
	
}

GLuint loadBitmapAsTexture(const char* path)
{
	int ASM_width, ASM_height;
	unsigned char* ASM_imageData = NULL;
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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -5.0f);
	
	if(ASM_keyPressed == 1)
	{
		glBindTexture(GL_TEXTURE_2D, ASM_texture_smiley);	
		glBegin(GL_QUADS);
			glColor3f(1.0f,1.0f,1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, 1.0f, 0.0f);

			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, 1.0f, 0.0f);

			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, 0.0f);

			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, 0.0f);
		glEnd();
	
	}
	else if (ASM_keyPressed == 2)
	{
		glBindTexture(GL_TEXTURE_2D, ASM_texture_smiley);	
		glBegin(GL_QUADS);
			glColor3f(1.0f,1.0f,1.0f);
			glTexCoord2f(0.5f, 0.5f);
			glVertex3f(1.0f, 1.0f, 0.0f);

			glTexCoord2f(0.0f, 0.5f);
			glVertex3f(-1.0f, 1.0f, 0.0f);

			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, 0.0f);

			glTexCoord2f(0.5f, 1.0f);
			glVertex3f(1.0f, -1.0f, 0.0f);
		glEnd();
	
	}
	else if (ASM_keyPressed == 3)
	{
		glBindTexture(GL_TEXTURE_2D, ASM_texture_smiley);	
		glBegin(GL_QUADS);
			glColor3f(1.0f,1.0f,1.0f);
			glTexCoord2f(2.0f, 0.0f);
			glVertex3f(1.0f, 1.0f, 0.0f);

			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, 1.0f, 0.0f);

			glTexCoord2f(0.0f, 2.0f);
			glVertex3f(-1.0f, -1.0f, 0.0f);

			glTexCoord2f(2.0f, 2.0f);
			glVertex3f(1.0f, -1.0f, 0.0f);
		glEnd();
	
	}
	else if (ASM_keyPressed == 4)
	{
		glBindTexture(GL_TEXTURE_2D, ASM_texture_smiley);	
		glBegin(GL_QUADS);
			glColor3f(1.0f,1.0f,1.0f);
			glTexCoord2f(0.5f, 0.5f);
			glVertex3f(1.0f, 1.0f, 0.0f);

			glTexCoord2f(0.5f, 0.5f);
			glVertex3f(-1.0f, 1.0f, 0.0f);

			glTexCoord2f(0.5f, 0.5f);
			glVertex3f(-1.0f, -1.0f, 0.0f);

			glTexCoord2f(0.5f, 0.5f);
			glVertex3f(1.0f, -1.0f, 0.0f);
		glEnd();
	
	}
	 else
	{	
		glBegin(GL_QUADS);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, 0.0f);
			glVertex3f(-1.0f, 1.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, 0.0f);
		glEnd();
	}
	
	
	
	
	glXSwapBuffers(ASM_gpDisplay, ASM_gWIndow);

}


void Uninitialize(void)
{
	GLXContext ASM_currentGLXCOntext;
	ASM_currentGLXCOntext = glXGetCurrentContext();
	
	if (ASM_texture_smiley)
	{
		glDeleteTextures(1, &ASM_texture_smiley);
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

