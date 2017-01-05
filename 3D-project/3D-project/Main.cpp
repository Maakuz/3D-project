#include "WindowClass.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	WindowClass windClass(hInstance, 640, 480);


	return windClass.run();
}
