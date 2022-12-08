//Headers
#include <windows.h>
#include "icon.h"

//pragma
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")

//global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("ASM App");
	int ASMXCenterCoordinate;
	int ASMYCenterCoordinate;
	int ASMWindowWidth = 800;
	int ASMWindowHeight = 600;

	//code
	//initialzation of WNDCLASSEX

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ASMICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(ASMICON));

	//register above class
	RegisterClassEx(&wndclass);

	//Window Mid of Screen 
	ASMXCenterCoordinate = ( GetSystemMetrics( SM_CXSCREEN ) / 2) - ( ASMWindowWidth / 2 ) ;
	ASMYCenterCoordinate = (GetSystemMetrics(SM_CYSCREEN) / 2) - (ASMWindowHeight / 2);

	//create window

	hwnd = CreateWindow(szAppName,
		TEXT("Akshay Mane MessageBox using WS_PRINTF"),
		WS_OVERLAPPEDWINDOW,
		ASMXCenterCoordinate,
		ASMYCenterCoordinate, 
		ASMWindowWidth, 
		ASMWindowHeight,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	//message loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return((int)msg.wParam);

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

	//local varaiable declaration
	int ASM_num1 = 75, ASM_num2= 25, ASM_num3;
	TCHAR ASM_str[255];

	//code
	switch (iMsg)
	{

	case WM_CREATE:

		ASM_num3 = ASM_num1 + ASM_num2;
		
		wsprintf(ASM_str, TEXT("Sum of %d and %d is %d"),ASM_num1,ASM_num2,ASM_num3);
		MessageBox(hwnd, ASM_str,TEXT("My Sum"), MB_OK);

	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}
