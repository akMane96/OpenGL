#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

////namespace
using namespace std;

////global varaiable declarations

bool bFullscreen = false;
Display *gpDisplay = NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;
int giWindowWidth = 800;
int giWindowHeight = 600;


int main(void)
{

	void CreateWindow(void);
	void ToggleFullScreen(void);
	void uninitialize();
	
	int winWidth = giWindowWidth;
	int winHeight = giWindowHeight;
	static XFontStruct *pxFontStruct = NULL;
	static GC gc;
	XGCValues gcValues;
	XColor greenColor;
	char str[] = "Hello World";
	int strLength;
	int strWidth;
	int fontHeight;
	
	
	//code
	CreateWindow();
	
	//MEssageLoop
	XEvent event;
	KeySym keysym;
	
	while(1)
	{
		XNextEvent(gpDisplay,&event);
		
		switch(event.type)
		{
			case MapNotify:
				pxFontStruct = XLoadQueryFont(gpDisplay,"fixed");
				break;
			case KeyPress:
				keysym = XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode,0,0);
				switch(keysym)
				{
					case XK_Escape:
						XUnloadFont(gpDisplay, pxFontStruct->fid);
						XFreeGC(gpDisplay, gc);
						uninitialize();
						exit(0);
					case XK_F:
					case XK_f:
						if(bFullscreen == false)
						{
							ToggleFullScreen();
							bFullscreen = true;
						}
						else
						{
							ToggleFullScreen();
							bFullscreen = false;
						}
						break;
						
					default:
						break;					
					
				}
				break;
				
			case ButtonPress:
				switch(event.xbutton.button)
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
				winWidth = event.xconfigure.width;
				winHeight = event.xconfigure.height;
				break;
				
			case Expose:
				gc = XCreateGC(gpDisplay, gWindow, 0, &gcValues);
				XSetFont(gpDisplay, gc , pxFontStruct->fid);
				XAllocNamedColor(gpDisplay, gColormap, "green", &greenColor,&greenColor); 
				XSetForeground(gpDisplay, gc, greenColor.pixel);
				strLength = strlen(str);
				strWidth = XTextWidth(pxFontStruct, str, strLength);
				fontHeight = pxFontStruct->ascent + pxFontStruct->descent;
				
				XDrawString(gpDisplay, gWindow, gc, (winWidth/2 - strWidth/2),(winHeight/2 - fontHeight/2),str,strLength);
				break;
			
			case DestroyNotify:
				break;
				
			case 33: 
				XUnloadFont(gpDisplay, pxFontStruct->fid); // fid or not
				XFreeGC(gpDisplay, gc);
				uninitialize();
				exit(0);
			default:
				break;
		}
				
	}
	
	uninitialize();
	return(0);
}

void CreateWindow(void)
{
	void uninitialize(void);

	////varaiable declarations
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int defaultDepth;
	int styleMask;

	//code
	gpDisplay = XOpenDisplay(NULL);
	
	if(gpDisplay == NULL)
	{
		printf("ERROR : Unable To Open X Display.....Exitting Now\n");
		uninitialize();
		exit(1);
	}

	defaultScreen = XDefaultScreen(gpDisplay);

	defaultDepth = DefaultDepth(gpDisplay, defaultScreen);		
	
	gpXVisualInfo = (XVisualInfo *)malloc(sizeof(XVisualInfo));

	if(gpXVisualInfo == NULL)
	{
 		printf("ERROR : Unable To ALLocate Memory for Visual Info.....Exitting Now\n");
		uninitialize();
		exit(1);	
	}
	
	XMatchVisualInfo(gpDisplay,defaultScreen,defaultDepth,TrueColor,gpXVisualInfo);
	
	if(gpXVisualInfo == NULL)
	{
 		printf("ERROR : Unable To Get Visual Info.....Exitting Now\n");
		uninitialize();
		exit(1);	
	}

	winAttribs.border_pixel = 0;
	winAttribs.background_pixmap = 0;
	winAttribs.colormap = XCreateColormap(gpDisplay, 
						RootWindow(gpDisplay, gpXVisualInfo ->screen),
						gpXVisualInfo->visual,
						AllocNone);
	gColormap = winAttribs.colormap;
	
	winAttribs.background_pixel = BlackPixel(gpDisplay, defaultScreen);

	winAttribs.event_mask = ExposureMask | VisibilityChangeMask | ButtonPressMask | 		KeyPressMask| PointerMotionMask | StructureNotifyMask;

	styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;

	gWindow = XCreateWindow ( gpDisplay,
				RootWindow(gpDisplay,gpXVisualInfo->screen),
				0,
				0,
				giWindowWidth,
				giWindowHeight,
				0,
				gpXVisualInfo->depth,
				InputOutput,
				gpXVisualInfo->visual,
				styleMask,
				&winAttribs);
	if(!gWindow)
	{
 		printf("ERROR : Failed to Craete Main Window\n");
		uninitialize();
		exit(1);	
	}
	
	XStoreName(gpDisplay, gWindow,"Akshay XWindow");
	
	Atom windowManagerDelete = XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(gpDisplay, gWindow,&windowManagerDelete, 1);
	
	XMapWindow(gpDisplay, gWindow);
	
}
void ToggleFullScreen(void)
{

	Atom wm_state;
	Atom fullscreen;
	XEvent xev = {xev};
	
	//code
	wm_state = XInternAtom(gpDisplay, "_NET_WM_STATE",False);
	memset(&xev, 0, sizeof(xev));
	
	xev.type = ClientMessage;
	xev.xclient.window = gWindow;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0]= bFullscreen ? 0:1;
	
	fullscreen = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
	xev.xclient.data.l[1] = fullscreen;
	
	XSendEvent(gpDisplay,
		RootWindow(gpDisplay,gpXVisualInfo->screen),
		False,
		StructureNotifyMask,
		&xev);
}
void uninitialize()
{

	if(gWindow)
	{
		XDestroyWindow(gpDisplay, gWindow);
	}
	if(gColormap)
	{
		XFreeColormap(gpDisplay, gColormap);
	}
	
	if(gpXVisualInfo)
	{
		free(gpXVisualInfo);
		gpXVisualInfo = NULL;
	}
	
	if(gpDisplay)
	{
		XCloseDisplay(gpDisplay);
		gpDisplay = NULL;
	}


}



