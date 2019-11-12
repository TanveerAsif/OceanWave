#include "Dx11_Engine.h"


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static Dx11_Engine *g_pEngineHandler;


Dx11_Engine::Dx11_Engine()
{
	m_hInstance = NULL;
	m_hWnd = NULL;

	m_pGraphics = NULL;
	m_pInput = NULL;
}


Dx11_Engine::~Dx11_Engine()
{
}


bool Dx11_Engine::Initialize()
{
	bool bRetValue = false;
	if (InitializeWindow())
	{
		m_pGraphics = new Dx11_Graphics();
		m_pInput = new Dx11_Input();

		if (m_pGraphics && m_pInput)
		{
			m_pInput->Initialize();
			
			if (m_pGraphics->Initialize(m_hWnd))
			{
				bRetValue = true;
			}			
		}
	}

	return bRetValue;
}


void Dx11_Engine::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));


	//NOTE : Winmm.lib ADD INTO LINKER TO RESOLVE timeGetTime() 
	unsigned int tickCount = 0, lastTickCount = timeGetTime();
	

	

	while(true)
	{
		tickCount = timeGetTime();
		float fTickDiff = (float(tickCount - lastTickCount)) / 1000;
		lastTickCount = tickCount;

		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		

		if (msg.message == WM_QUIT)
			break;


		if (m_pInput->IsKeyDown(VK_ESCAPE))
			break;

		if (m_pInput->IsKeyDown('W'))
		{
			m_pInput->KeyUp('W');
			m_pGraphics->ShowWireFrame();
		}
		
		if (m_pInput->IsKeyDown('D'))
		{
			m_pInput->KeyUp('D');
			m_pGraphics->ShowDepthMap();
		}

		//SUN ROTATION
		if (m_pInput->IsKeyDown('M'))
		{
			m_pInput->KeyUp('M');
			m_pGraphics->ChangeSunDirection('M');
		}

		if (m_pInput->IsKeyDown('N'))
		{
			m_pInput->KeyUp('N');
			m_pGraphics->ChangeSunDirection('N');
		}
		if (m_pInput->IsKeyDown('H'))
		{
			m_pInput->KeyUp('H');
			m_pGraphics->ChangeSunDirection('H');
		}

		if (m_pInput->IsKeyDown('J'))
		{
			m_pInput->KeyUp('J');
			m_pGraphics->ChangeSunDirection('J');
		}





		if (m_pInput->IsKeyDown('R'))
		{
			m_pInput->KeyUp('R');
			m_pGraphics->ChangeSunDirection('R');
		}

		if (m_pInput->IsKeyDown('L'))
		{
			m_pInput->KeyUp('L');
			m_pGraphics->SunLightOnOff();
		}

		if (m_pInput->IsKeyDown('O'))
		{
			m_pInput->KeyUp('O');
			m_pGraphics->LightShaftOnOff();
		}
		if (m_pInput->IsKeyDown(VK_LEFT))
		{
			m_pInput->KeyUp(VK_LEFT);
			m_pGraphics->UpdateCamera(VK_LEFT);
		}

		if (m_pInput->IsKeyDown(VK_RIGHT))
		{
			m_pInput->KeyUp(VK_RIGHT);
			m_pGraphics->UpdateCamera(VK_RIGHT);
		}

		if (m_pInput->IsKeyDown(VK_UP))
		{
			m_pInput->KeyUp(VK_UP);
			m_pGraphics->UpdateCamera(VK_UP);
		}

		if (m_pInput->IsKeyDown(VK_DOWN))
		{
			m_pInput->KeyUp(VK_DOWN);
			m_pGraphics->UpdateCamera(VK_DOWN);
		}
		
		if (m_pInput->IsKeyDown(VK_PRIOR))
		{
			m_pInput->KeyUp(VK_PRIOR);
			m_pGraphics->UpdateCamera(VK_PRIOR);
		}

		if (m_pInput->IsKeyDown(VK_NEXT))
		{
			m_pInput->KeyUp(VK_NEXT);
			m_pGraphics->UpdateCamera(VK_NEXT);
		}

		if (msg.message == WM_MOUSEWHEEL)
		{			
			m_pGraphics->UpdateCamera('T');
		}

		m_pGraphics->Process(fTickDiff);
		m_pGraphics->Render(fTickDiff);

	}

}



bool Dx11_Engine::InitializeWindow()
{

	g_pEngineHandler = this;

	m_hInstance = GetModuleHandle(NULL);


	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"Engine";
	wc.cbSize = sizeof(WNDCLASSEX);
	if (!RegisterClassEx(&wc))
		return false;


	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, L"Engine", L"Engine", WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP
		, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)
		, 0, 0, m_hInstance, NULL);
	if (m_hWnd == NULL)
		return false;

	ShowWindow(m_hWnd, SW_SHOW);
	return true;
}

LRESULT CALLBACK Dx11_Engine::MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{

	case WM_KEYDOWN:
		m_pInput->KeyDown((unsigned int)wParam);
		break;

	case WM_KEYUP:
		m_pInput->KeyUp((unsigned int)wParam);
		break;

	case WM_MOUSEWHEEL:
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
		
	}

}




LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{

	default:
		return g_pEngineHandler->MessageHandler(hWnd, msg, wParam, lParam);
	}
}

void Dx11_Engine::Shutdown()
{
	if (m_pGraphics)
	{
		m_pGraphics->Shutdown();
		delete m_pGraphics;
		m_pGraphics = NULL;
	}


	if (m_pInput)
	{
		delete m_pInput;
		m_pInput = NULL;
	}

}