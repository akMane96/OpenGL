#include <windows.h>
#include <stdio.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

FILE* gpFile = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//variable declaration
	WNDCLASSEX ASM_wndclass;
	HWND ASM_hwnd;
	MSG ASM_msg;
	TCHAR szAppName[] = TEXT("ASM Appication");

	//code

	if (fopen_s(&gpFile, "ASMLog.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("File Creating Failed!!!!!"), TEXT("Error"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	fprintf(gpFile, "\n--------- # Log File GUI#-------by------# Akshay Mane #\n\n");
	
	//Initailization of WNDCLASSEX
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

	//Create window
	ASM_hwnd = CreateWindow(szAppName, TEXT("ASM FileIO Window"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	ShowWindow(ASM_hwnd, iCmdShow);
	UpdateWindow(ASM_hwnd);

	//Message loop
	while (GetMessage(&ASM_msg, NULL, 0, 0))
	{
		TranslateMessage(&ASM_msg);
		DispatchMessage(&ASM_msg);
	}

	fclose(gpFile);
	gpFile = NULL;

	return((int)ASM_msg.wParam);
}

LRESULT CALLBACK WndProc(HWND ASM_hwnd, UINT ASM_iMsg, WPARAM ASM_wParam, LPARAM ASM_lParam)
{
	//code
	switch (ASM_iMsg)
	{

	case WM_CREATE:
		fprintf(gpFile, "India Is My Country.\n");
		break;

	case WM_DESTROY:
		fprintf(gpFile, "Jai Hind!!!\n");
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(ASM_hwnd, ASM_iMsg, ASM_wParam, ASM_lParam));
}