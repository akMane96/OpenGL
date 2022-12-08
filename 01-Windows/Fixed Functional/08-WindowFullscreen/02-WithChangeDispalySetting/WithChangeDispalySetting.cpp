//header files
#include <windows.h>
#include <stdio.h>


//Global Function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global Varaiable declaration
DWORD ASM_dwStyle;
bool ASM_gbFullScreen = false;
HWND ASM_ghwnd = NULL;
FILE* ASM_gpFile = NULL;
int iASMWindowWidth = 800;
int iASMWindowHeight = 600;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//local varaibale declaration
	WNDCLASSEX ASM_wndclass;
	HWND ASM_hwnd;
	MSG ASM_msg;
	TCHAR szAppName[] = TEXT("ASM Appication");
	int iASMXCenterCoordinate;
	int iASMYCenterCoordinate;
	

	////code
	fopen_s(&ASM_gpFile, "Log.txt", "w");

	ASM_wndclass.cbSize = sizeof(WNDCLASSEX);
	ASM_wndclass.style = CS_HREDRAW | CS_VREDRAW;
	ASM_wndclass.cbClsExtra = 0;
	ASM_wndclass.cbWndExtra = 0;
	ASM_wndclass.lpfnWndProc = WndProc;
	ASM_wndclass.hInstance = hInstance;
	ASM_wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	ASM_wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	ASM_wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	ASM_wndclass.lpszClassName = szAppName;
	ASM_wndclass.lpszMenuName = NULL;
	ASM_wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	//Register Window
	RegisterClassEx(&ASM_wndclass);

	//Window Mid of Screen 
	iASMXCenterCoordinate = (GetSystemMetrics(SM_CXSCREEN) / 2) - (iASMWindowWidth / 2);
	iASMYCenterCoordinate = (GetSystemMetrics(SM_CYSCREEN) / 2) - (iASMWindowHeight / 2);
	
	//create window
	ASM_hwnd = CreateWindow(szAppName, TEXT("ASM FullScreen Window"),
		WS_OVERLAPPEDWINDOW,
		iASMXCenterCoordinate,
		iASMYCenterCoordinate,
		iASMWindowWidth,
		iASMWindowHeight,
		NULL,
		NULL,
		hInstance,
		NULL);

	ASM_ghwnd = ASM_hwnd;

	ShowWindow(ASM_hwnd, iCmdShow);
	UpdateWindow(ASM_hwnd);

	//message loop
	while (GetMessage(&ASM_msg, NULL, 0, 0))
	{
		TranslateMessage(&ASM_msg);
		DispatchMessage(&ASM_msg);
	}
	fclose(ASM_gpFile);

	return((int)ASM_msg.wParam);
}

LRESULT CALLBACK WndProc(HWND ASM_hwnd, UINT ASM_iMsg, WPARAM ASM_wParam, LPARAM ASM_lParam)
{

	//Local Function declaration
	void ASM_ToggleFullScreen(void);

	//Local Varaiable Declaration
	HDC ASM_hdc;
	PAINTSTRUCT ASM_ps;
	RECT ASM_rc;
	TCHAR ASM_str[] = TEXT("Akshay Window Fullscreen Using ChangeDisplaySettings() !!!!!!! ");

	//code
	switch (ASM_iMsg)
	{
	case WM_PAINT:
		GetClientRect(ASM_hwnd, &ASM_rc);
		ASM_hdc = BeginPaint(ASM_hwnd, &ASM_ps);

		SetBkColor(ASM_hdc, RGB(0, 0, 0));
		SetTextColor(ASM_hdc, RGB(255, 0, 0));

		DrawText(ASM_hdc,
			ASM_str,
			-1,
			&ASM_rc,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		EndPaint(ASM_hwnd, &ASM_ps);
		break;


	case WM_KEYDOWN:
		switch (ASM_wParam)
		{
		case 0X46:
		case 0X66:
			ASM_ToggleFullScreen();
			break;

		default:
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(ASM_hwnd, ASM_iMsg, ASM_wParam, ASM_lParam));
}

void ASM_ToggleFullScreen(void)
{
	//Local varaiable declaration;
	DEVMODE ASM_mode = { sizeof(DEVMODE) };
	ASM_dwStyle = GetWindowLong(ASM_ghwnd, GWL_STYLE);

	
	if (ASM_gbFullScreen == false)
	{
		ASM_mode.dmSize = sizeof(ASM_mode);
		ASM_mode.dmPelsWidth = iASMWindowWidth;
		ASM_mode.dmPelsHeight = iASMWindowHeight;
		ASM_mode.dmBitsPerPel = 32;
		ASM_mode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

		
		if (ASM_dwStyle & WS_OVERLAPPEDWINDOW)
		{
			SetWindowLong(ASM_ghwnd, GWL_STYLE, (ASM_dwStyle & ~WS_OVERLAPPEDWINDOW));
		}


		if (ChangeDisplaySettings(&ASM_mode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			ASM_gbFullScreen = false;
			SetWindowLong(ASM_ghwnd, GWL_STYLE, (ASM_dwStyle | WS_OVERLAPPEDWINDOW));
			ChangeDisplaySettings(NULL, 0);
			fprintf(ASM_gpFile, " Not Sucess\n");
		}
		else
		{
			ASM_gbFullScreen = true;
			fprintf(ASM_gpFile, "Sucess gbFullscreen : %d\n", ASM_gbFullScreen);
		}
		
	}
	else
	{
		
		SetWindowLong(ASM_ghwnd, GWL_STYLE, (ASM_dwStyle | WS_OVERLAPPEDWINDOW));
		ChangeDisplaySettings(NULL, 0);
		ASM_gbFullScreen = false;
		fprintf(ASM_gpFile, "Exit Full Screen gbFullscreen : %d\n", ASM_gbFullScreen);

	}
}
	

