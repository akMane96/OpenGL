#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	WNDCLASSEX ASM_wndclass;
	HWND ASM_hwnd;
	MSG ASM_msg;
	TCHAR szAppName[] = TEXT("ASM Appication");
	int ASMXCenterCoordinate;
	int ASMYCenterCoordinate;
	int ASMWindowWidth = 800;
	int ASMWindowHeight = 600;

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
	ASMXCenterCoordinate = ( GetSystemMetrics( SM_CXSCREEN ) / 2) - ( ASMWindowWidth / 2 ) ;
	ASMYCenterCoordinate = (GetSystemMetrics(SM_CYSCREEN) / 2) - (ASMWindowHeight / 2);

	//craete window
	ASM_hwnd = CreateWindow(szAppName,TEXT("ASM Centered Window"),WS_OVERLAPPEDWINDOW, ASMXCenterCoordinate, ASMYCenterCoordinate, ASMWindowWidth, ASMWindowHeight,NULL,NULL,hInstance,NULL);
	
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

	switch (ASM_iMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(ASM_hwnd, ASM_iMsg, ASM_wParam, ASM_lParam));
}