#include "Dx11_Engine.h"



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdShow)
{

	Dx11_Engine *pEngine = new Dx11_Engine();

	if (pEngine->Initialize())
	{
		pEngine->Run();
	}
	pEngine->Shutdown();

	return 0;
}