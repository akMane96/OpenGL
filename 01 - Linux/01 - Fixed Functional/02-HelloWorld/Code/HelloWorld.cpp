#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

using namespace std;

//Global Varaiable Declaration

bool ASM_bfullscreen = false;
Display *ASM_gpDisplay = NULL;
XVisualInfo *ASM_gpXVisualInfo = NULL;
Colormap ASM_gColormap;
Window ASM_gWindow;
int ASM_giWindowWidth = 800;
int ASM_giWindowHeight = 600;



int main(void)
{

	void CreateWindow(void);
	void ToggleFullScreen(void);
	void Uninitialize();
	
	
	int ASM_winWidth = ASM_giWindowWidth;
	int ASM_winHeight = ASM_giWindowHeight;
	static XFontStruct *ASM_pxFontStruct = NULL;
	static GC ASM_gc;
	XGCValues ASM_gcValues;
	XColor ASM_greenColor;
	char ASM_str[] = "Hello XWindow from Akshay Mane";
	int ASM_strLength;
	int ASM_strWidth;
	int ASM_fontHeight;
		
	//code
	CreateWindow();
	
	//MEssage Loop
	
	XEvent ASM_event;
	KeySym ASM_keysym;
	
	while(1)
	
	while(1)
	{
		XNextEvent(ASM_gpDisplay,&ASM_event);
		
		switch(ASM_event.type)
		{
			case MapNotify:
				ASM_pxFontStruct = XLoadQueryFont(ASM_gpDisplay, "fixed");
				break;
				
			case KeyPress:
				ASM_keysym = XkbKeycodeToKeysym(ASM_gpDisplay, ASM_event.xkey.keycode,0,0);
				switch(ASM_keysym)
				{
					case XK_Escape:
						XUnloadFont(ASM_gpDisplay, ASM_pxFontStruct->fid);
						XFreeGC(ASM_gpDisplay, ASM_gc);
						Uninitialize();
						exit(0);
					
					case XK_F:
					case XK_f:
						if(ASM_bfullscreen == false)
						{
							ToggleFullScreen();
							ASM_bfullscreen = true;
						}
						else
						{
							ToggleFullScreen();
							ASM_bfullscreen = false;
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
				ASM_winWidth =  ASM_event.xconfigure.width;
				ASM_winHeight= ASM_event.xconfigure.height;
				break;
				
		/*int ASM_winWidth = ASM_giWindowWidth;
		int ASM_winHeight = ASM_giWindowHeight;
		static XFontStruct *ASM_pxFontStruct = NULL;
		static GC ASM_gc;
		XGCValues ASM_gcValues;
		XColor ASM_greenColor;
		char str[] = "XWindow Akshay Mane";
		int ASM_strLength;
	int ASM_strWidth;
	int ASM_fontHeight;
		;*/
		
		
/*bool ASM_bfullscreen = false;
Display *ASM_gpDisplay = NULL;
Colormap ASM_gColormap;
Window ASM_gWindow;
int ASM_giWindowWidth = 800;
int ASM_giWindowHeight = 600;*/
				
			case Expose:
			
				ASM_gc = XCreateGC(ASM_gpDisplay, ASM_gWindow, 0, &ASM_gcValues) ;
				XSetFont(ASM_gpDisplay, ASM_gc, ASM_pxFontStruct->fid);
				XAllocNamedColor(ASM_gpDisplay, ASM_gColormap, "green", &ASM_greenColor, &ASM_greenColor);
				XSetForeground(ASM_gpDisplay, ASM_gc, ASM_greenColor.pixel);
				ASM_strLength = strlen(ASM_str);
				ASM_strWidth = XTextWidth(ASM_pxFontStruct, ASM_str, ASM_strLength);
				ASM_fontHeight = ASM_pxFontStruct->ascent + ASM_pxFontStruct->descent;
				XDrawString(ASM_gpDisplay, ASM_gWindow, ASM_gc, (ASM_winWidth/2 - ASM_strWidth/2), (ASM_winHeight/2 - ASM_fontHeight/2), ASM_str, ASM_strLength);
				break;
				
			case DestroyNotify:
				break;
				
				case 33:
					XUnloadFont(ASM_gpDisplay, ASM_pxFontStruct->fid);
					XFreeGC(ASM_gpDisplay, ASM_gc);
					Uninitialize();
					exit(0);

					break;
					
			default:
				break;					
				
		}	
	
	}
	
	Uninitialize();
	return(0);
	

}

void CreateWindow()
{
	void Uninitialize();
	
	//Varaiable declrations
	XSetWindowAttributes ASM_winAttribs;
	int ASM_defaultScreen;
	int ASM_defaultDepth;
	int ASM_styleMask;
	
	//code
	ASM_gpDisplay = XOpenDisplay(NULL);
	
	if(ASM_gpDisplay == NULL)
	{
		printf("ERROR : Unable To Open X Display..Exitting Now\n");
		Uninitialize();
		exit(1);	
	}
	
	ASM_defaultScreen = XDefaultScreen(ASM_gpDisplay);
	
	ASM_defaultDepth = DefaultDepth(ASM_gpDisplay, ASM_defaultScreen);
	
	ASM_gpXVisualInfo = (XVisualInfo *)malloc(sizeof(XVisualInfo));
	
	if(ASM_gpXVisualInfo == NULL)
	{
		printf("ERROR : Unable To ALLocate Memory for Visual Info.....Exitting Now\n");
		Uninitialize();
		exit(1);	
	}
	
	XMatchVisualInfo(ASM_gpDisplay, ASM_defaultScreen, ASM_defaultDepth, TrueColor,ASM_gpXVisualInfo);
	
	if(ASM_gpXVisualInfo == NULL)
	{
 		printf("ERROR : Unable To Get Visual Info.....Exitting Now\n");
		Uninitialize();
		exit(1);	
	}
	
	ASM_winAttribs.border_pixel = 0;
	ASM_winAttribs.background_pixmap = 0;
	ASM_winAttribs.colormap = XCreateColormap(ASM_gpDisplay, RootWindow(ASM_gpDisplay, ASM_gpXVisualInfo->screen), ASM_gpXVisualInfo->visual, AllocNone);
	
	ASM_gColormap = ASM_winAttribs.colormap;
	
	ASM_winAttribs.background_pixel = BlackPixel(ASM_gpDisplay, ASM_defaultScreen);
	
	ASM_winAttribs.event_mask = ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;
	
	ASM_styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	
	ASM_gWindow = XCreateWindow (ASM_gpDisplay, 
					RootWindow(ASM_gpDisplay, ASM_gpXVisualInfo->screen),
					0,0, ASM_giWindowWidth, ASM_giWindowHeight, 0,
					ASM_gpXVisualInfo->depth,
					InputOutput,
					ASM_gpXVisualInfo->visual,
					ASM_styleMask,
					&ASM_winAttribs);
					
	if(!ASM_gWindow)
	{
 		printf("ERROR : Failed to Craete Main Window\n");
		Uninitialize();
		exit(1);	
	}
	
	XStoreName(ASM_gpDisplay, ASM_gWindow, "Akshay XWindow");
	
	Atom ASM_windowManagerDelete = XInternAtom(ASM_gpDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(ASM_gpDisplay, ASM_gWindow, &ASM_windowManagerDelete, 1);
	
	XMapWindow(ASM_gpDisplay, ASM_gWindow);

		




}

void ToggleFullScreen()
{

	Atom ASM_wm_state;
	Atom ASM_fullscreen;
	XEvent ASM_xev = {ASM_xev};
	
	ASM_wm_state = XInternAtom(ASM_gpDisplay, "_NET_WM_STATE", False);
	memset(&ASM_xev, 0, sizeof(ASM_xev));
	
	ASM_xev.type = ClientMessage;
	ASM_xev.xclient.window = ASM_gWindow;
	ASM_xev.xclient.message_type = ASM_wm_state;
	ASM_xev.xclient.format = 32;
	ASM_xev.xclient.data.l[0] = ASM_bfullscreen ? 0:1;
	
	ASM_fullscreen = XInternAtom (ASM_gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
	ASM_xev.xclient.data.l[1] = ASM_fullscreen;

	XSendEvent(ASM_gpDisplay,
	RootWindow(ASM_gpDisplay, ASM_gpXVisualInfo->screen),
	False,
	StructureNotifyMask,
	&ASM_xev);
}

void Uninitialize()
{

	if(ASM_gWindow)
	{
		XDestroyWindow(ASM_gpDisplay, ASM_gWindow);
	}
	
	if(ASM_gColormap)
	{
		XFreeColormap(ASM_gpDisplay,ASM_gColormap);
	}
	
	if(ASM_gpXVisualInfo)
	{
	
		free(ASM_gpXVisualInfo);
		ASM_gpXVisualInfo = NULL;
	}
	
	if(ASM_gpDisplay)
	{
	
		XCloseDisplay(ASM_gpDisplay);
		ASM_gpDisplay = NULL;
	}
}



























