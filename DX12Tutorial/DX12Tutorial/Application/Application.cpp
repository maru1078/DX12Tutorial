#include "Application.h"

#include "../DX12/Dx12.h"
#include "../SphereRenderer/SphereRenderer.h"
#include "../PeraPolygon/PeraPolygon.h"

LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

Application::Application(float windowWidth, float windowHeight)
	: m_windowWidth{ windowWidth }
	, m_windowHeight{ windowHeight }
{
}

void Application::Initialize()
{
	m_windowClass.lpszClassName = "DX12Tutorial";
	m_windowClass.hInstance = GetModuleHandle(nullptr);
	m_windowClass.lpfnWndProc = (WNDPROC)WndProc;
	m_windowClass.cbSize = sizeof(m_windowClass); // Exの場合、これがないとダメ
	RegisterClassEx(&m_windowClass);

	RECT rect{ 0, 0, m_windowWidth, m_windowHeight };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	m_hWnd = CreateWindow(
		m_windowClass.lpszClassName,
		m_windowClass.lpszClassName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		m_windowClass.hInstance,
		nullptr);

	ShowWindow(m_hWnd, SW_SHOW);

	m_dx12 = std::make_shared<Dx12>(m_windowWidth, m_windowHeight, m_hWnd);

	m_sphereRenderer = std::make_shared<SphereRenderer>(m_dx12);

	m_sphereRenderer->CreateSphere(
		XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f },
		1.0f
	);

	m_sphereRenderer->CreateSphere(
		XMFLOAT3{ 2.0f, 0.0f, 0.0f },
		XMFLOAT4{ 1.0f, 1.0f, 0.0f, 1.0f },
		0.5f
	);

	m_pera = std::make_shared<PeraPolygon>(m_dx12);
}

void Application::Run()
{
	Initialize();
	m_dx12->SetBackGroundColor(0.5f, 0.5f, 0.5f);
	float angle = 0.0f;

	MSG msg{};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		angle += 0.01f;

		// DirectX12の処理
		m_dx12->BeginDraw();

		m_dx12->Update();

		// とりあえず三角関数を使って無理やり移動
		m_dx12->SetEyePosition(XMFLOAT3{ std::sin(angle) * 5, 0.0f, -std::cos(angle) * 5 });

		m_sphereRenderer->Update();
		m_sphereRenderer->Draw();

		//m_pera->PreDrawPera();
		//m_dx12->SetDepthTexture();
		//m_pera->DrawPera();

		m_dx12->EndDraw();
	}
}

void Application::Finalize()
{
	UnregisterClass(m_windowClass.lpszClassName, m_windowClass.hInstance);
}
