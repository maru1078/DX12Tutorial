
#include <Windows.h>

#include "Application/Application.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Application app{ 1920.0f, 1080.0f };

	app.Run();

	return 0;
}