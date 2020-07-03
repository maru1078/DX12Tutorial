
#include <Windows.h>

#include "Application/Application.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Application app{ 960, 540.0f };

	app.Run();

	return 0;
}