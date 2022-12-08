#include<windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{

	WNDCLASSEX ASM_wndclass;
	HWND ASM_hwnd;
	MSG ASM_msg;
	TCHAR szAppName[] = TEXT("ASM Application");

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

	RegisterClassEx(&ASM_wndclass);

	ASM_hwnd = CreateWindow(szAppName, TEXT("Akshay WMPAINT WINDOW"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	ShowWindow(ASM_hwnd, iCmdShow);
	UpdateWindow(ASM_hwnd);

	while (GetMessage(&ASM_msg, NULL, 0, 0))
	{

		TranslateMessage(&ASM_msg);
		DispatchMessage(&ASM_msg);

	}

	return((int)ASM_msg.wParam);
}

LRESULT CALLBACK WndProc(HWND ASM_hwnd, UINT ASM_iMsg, WPARAM ASM_wParam, LPARAM ASM_lParam)
{
	HDC ASM_hdc;
	PAINTSTRUCT ASM_ps;
	RECT ASM_rc;
	TCHAR ASM_str[] = TEXT("Hello World !!!");

	switch (ASM_iMsg)
	{

	case WM_PAINT:

		GetClientRect(ASM_hwnd, &ASM_rc);
		ASM_hdc = BeginPaint(ASM_hwnd, &ASM_ps);
		SetBkColor(ASM_hdc, RGB(0, 0, 0));
		SetTextColor(ASM_hdc, RGB(0, 255, 0));
		DrawText(ASM_hdc, ASM_str, -1, &ASM_rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		EndPaint(ASM_hwnd, &ASM_ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);

		break;

	}

	return(DefWindowProc(ASM_hwnd, ASM_iMsg, ASM_wParam, ASM_lParam));
}