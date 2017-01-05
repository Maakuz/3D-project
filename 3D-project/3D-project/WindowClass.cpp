#include "WindowClass.h"

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


WindowClass::WindowClass(HINSTANCE hInstance)
{
	this->width = 640;
	this->heigth = 480;
	this->wnd = 0;
	this->hInstance = hInstance;
	this->initializeWindow();
	this->graphicsHandler = new GraphicsHandler(this->wnd, this->width, this->heigth);
	
}

WindowClass::WindowClass(HINSTANCE hInstance, int width, int height)
{
	this->width = width;
	this->heigth = height;
	this->wnd = 0;
	this->hInstance = hInstance;
	this->initializeWindow();
}

WindowClass::~WindowClass()
{
	delete graphicsHandler;
}

void WindowClass::initializeWindow()
{
	WNDCLASS wc = { 0 };

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = this->hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_HELP);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"Basic test";

	if (!RegisterClass(&wc))
	{
		MessageBox(this->wnd, L"registerClass failed", L"Error", MB_OK);
	}

	this->wnd = CreateWindow(
		L"Basic test",
		L"3D project",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		this->width,
		this->heigth,
		0,
		0,
		this->hInstance,
		0);
	
	if (this->wnd == 0)
	{
		MessageBox(this->wnd, L"window creation failed", L"Error", MB_OK);
	}
		
	

	ShowWindow(this->wnd, SW_SHOWDEFAULT);
	UpdateWindow(this->wnd);

}


int WindowClass::run()
{
	MSG msg = { 0 };

	bool ret = true;

	while ((ret = GetMessage(&msg, 0, 0, 0)) != 0)
	{
		if (ret == -1)
		{
			MessageBox(this->wnd, L"message fetching failed", L"Error", MB_OK);
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}
