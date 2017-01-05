#ifndef WINDOWCLASS_H
#define WINDOWCLASS_H

#include <Windows.h>

class WindowClass
{
private:
	HWND wnd;
	int width;
	int heigth;
	HINSTANCE hInstance;

public:
	WindowClass(HINSTANCE hInstance);
	WindowClass(HINSTANCE hInstance, int width, int height);
	virtual ~WindowClass();

	void initializeWindow();

	int run();
};


#endif