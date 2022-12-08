//header files
#include <windows.h>

//Global Function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global Varaiable declaration
DWORD ASM_dwStyle;
WINDOWPLACEMENT ASM_wpPrev = { sizeof(WINDOWPLACEMENT) };
bool ASM_gbFullScreen = false;
HWND ASM_ghwnd = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//local varaibale declaration
	WNDCLASSEX ASM_wndclass;
	HWND ASM_hwnd;
	MSG ASM_msg;
	TCHAR szAppName[] = TEXT("ASM Appication");
	int iASMXCenterCoordinate;
	int iASMYCenterCoordinate;
	int iASMWindowWidth = 800;
	int iASMWindowHeight = 600;

	////code
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

	//craete window
	ASM_hwnd = CreateWindow(szAppName, TEXT("ASM Centered Window"),
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
	TCHAR ASM_str[] = TEXT("Akshay Window Fullscreen !!!!!!! ");

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
	//Local Function declaration
	MONITORINFO ASM_mi = { sizeof(MONITORINFO) };

	//code
	if (ASM_gbFullScreen == false)
	{
		ASM_dwStyle = GetWindowLong(ASM_ghwnd, GWL_STYLE);

		if (ASM_dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ASM_ghwnd, &ASM_wpPrev) &&
				GetMonitorInfo(MonitorFromWindow(ASM_ghwnd, MONITORINFOF_PRIMARY), &ASM_mi))
			{

				SetWindowLong(ASM_ghwnd,GWL_STYLE, (ASM_dwStyle & ~WS_OVERLAPPEDWINDOW));

				SetWindowPos(ASM_ghwnd,
					HWND_TOP, 
					ASM_mi.rcMonitor.left,
					ASM_mi.rcMonitor.top, 
					(ASM_mi.rcMonitor.right - ASM_mi.rcMonitor.left), 
					(ASM_mi.rcMonitor.bottom - ASM_mi.rcMonitor.top),
					SWP_NOZORDER | SWP_FRAMECHANGED);


			}
		}

		ShowCursor(FALSE);
		ASM_gbFullScreen = true;

	}
	else
	{
		SetWindowLong(ASM_ghwnd, GWL_STYLE, (ASM_dwStyle | WS_OVERLAPPEDWINDOW));

		SetWindowPlacement(ASM_ghwnd, &ASM_wpPrev);

		SetWindowPos(ASM_ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
		ASM_gbFullScreen = false;
		
	}


}
