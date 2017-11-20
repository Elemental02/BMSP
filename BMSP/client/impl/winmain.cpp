#include "../../stdafx.h"
#include <Windows.h>

static int width = 800;

static int height = 600;

static int FPSrates = 60;

static bool windowed = true;

bool Init(HINSTANCE, int, HWND&);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd = 0;
	HINSTANCE hInst = hInstance;
	if (!Init(hInst, nCmdShow, hWnd))
	{
		MessageBox(NULL, TEXT("Window Init Failed!"), TEXT("Error"), NULL);
		return 0;
	}

	MSG msg;

	ZeroMemory(&msg, sizeof(MSG));

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//game logic
		}
	}
	return msg.wParam;
}


bool Init(HINSTANCE hInst, int nCmdShow, HWND& hWnd)
{

	// Prepare window class
	WNDCLASS wndcls;

	// Memory cleanup
	ZeroMemory(&wndcls, sizeof(WNDCLASS));
	//wndcls.cbSize = sizeof(WNDCLASSEX);			// size of WNDCLASSEX
	wndcls.style =
		CS_HREDRAW |
		CS_VREDRAW;								// window style
	wndcls.lpfnWndProc = WndProc;	// window procedure
	wndcls.cbClsExtra = 0;						// number of extra bytes
	wndcls.cbWndExtra = 0;						// number of extra bytes
	wndcls.hInstance = hInst;				// instance handle of DLL
	wndcls.hIcon = LoadIcon(0, IDI_APPLICATION);						// icon of window
																		//wndcls.hIconSm = NULL;						// small icon of window
	wndcls.hCursor =
		LoadCursor(NULL, IDC_ARROW);			// cursor of window
	wndcls.hbrBackground = NULL;				// background of window
	wndcls.lpszClassName = TEXT("BMSP");		// class name of window class
	wndcls.lpszMenuName = NULL;					// menu of window

												// Register window class
	if (!RegisterClass(&wndcls))
		return false;

	hWnd = CreateWindow(TEXT("BMSP"), TEXT("BMSP"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hInst, 0);

	DWORD k = GetLastError();

	if (hWnd == 0)
		return false;

	ShowWindow(hWnd, nCmdShow);
	//UpdateWindow(hWnd);


	return true;

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	}

	return DefWindowProcW(hWnd, msg, wParam, lParam);
}