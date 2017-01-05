#include <Windows.h>

HWND wnd = 0;


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc = { 0 };

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_IBEAM);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"Basic test";

	RegisterClass(&wc);

	wnd = CreateWindow(
		L"Basic test", 
		L"3D project", 
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		640,
		480,
		0,
		0,
		hInstance,
		0);

	ShowWindow(wnd, nCmdShow);
	UpdateWindow(wnd);

	MSG msg = {0};

	bool ret = true;

	while ((ret = GetMessage(&msg, 0, 0, 0)) != 0)
	{
		if (ret == -1)
		{
			
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
		break;
	}

	return 0;
}
