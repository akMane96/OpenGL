//headers
#include <Windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow )
{

//varaible declaration
	WNDCLASSEX ASM_wndclass;
	HWND ASM_hwnd;
	MSG ASM_msg;
	TCHAR szAppName[] = TEXT("ASMApplication");

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

	//register above class
	RegisterClassEx(&ASM_wndclass);

	//create window
	ASM_hwnd = CreateWindow(szAppName,
		TEXT("AKSHAY WINDOW"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL );

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
	//code
	switch (ASM_iMsg)
	{
	case WM_CREATE:
		MessageBox(ASM_hwnd, TEXT("WM_CREATE Is Received "), TEXT("My Message"),MB_OK);
		break;
	case WM_LBUTTONDOWN:
		MessageBox(ASM_hwnd, TEXT("WM_LBUTTONDOWN Is Received "), TEXT("My Message"), MB_OK);
		break;
	case WM_RBUTTONDOWN:
		MessageBox(ASM_hwnd, TEXT("WM_RBUTTONDOWN Is Received "), TEXT("My Message"), MB_OK);
		break;

	case WM_KEYDOWN:
		MessageBox(ASM_hwnd, TEXT("WM_KEYDOWN Is Received "), TEXT("My Message"), MB_OK);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(ASM_hwnd, ASM_iMsg, ASM_wParam, ASM_lParam));

}