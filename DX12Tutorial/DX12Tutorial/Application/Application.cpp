#include "Application.h"

#include "../DX12/Dx12.h"
#include "../SphereRenderer/SphereRenderer.h"
#include "../SphereData/SphereData.h"
#include "../PeraPolygon/PeraPolygon.h"
#include "../LineDrawer/LineDrawer.h"
#include "../Keyboard/Keyboard.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

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

	auto s1 = m_sphereRenderer->CreateSphere(
		XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f },
		1.0f
	);

	auto s2 = m_sphereRenderer->CreateSphere(
		XMFLOAT3{ -2.0f, 3.0f, 1.0f },
		XMFLOAT4{ 0.0f, 0.0f, 1.0f, 1.0f },
		0.1f
	);

	auto s3 = m_sphereRenderer->CreateSphere(
		XMFLOAT3{ 5.0f, 2.0f, 1.0f },
		XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f },
		0.1f
	);

	auto s4 = m_sphereRenderer->CreateSphere(
		XMFLOAT3{ -3.0f, -2.0f, 0.0f },
		XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f },
		0.1f
	);

	auto s5 = m_sphereRenderer->CreateSphere(
		XMFLOAT3{ -5.0f, 1.0f, 2.0f },
		XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f },
		0.1f
	);

	auto s6 = m_sphereRenderer->CreateSphere(
		XMFLOAT3{ 3.0f, -1.0f, -2.0f },
		XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f },
		0.1f
	);

	auto s7 = m_sphereRenderer->CreateSphere(
		XMFLOAT3{ 2.0f, -3.0f, 1.0f },
		XMFLOAT4{ 0.0f, 0.0f, 1.0f, 1.0f },
		0.1f
	);

	m_lineDrawer = std::make_shared<LineDrawer>(m_dx12, XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f });
	m_lineDrawer->AddPosition(s2.lock()->position);
	m_lineDrawer->AddPosition(s3.lock()->position);
	m_lineDrawer->AddPosition(s4.lock()->position);
	m_lineDrawer->AddPosition(s5.lock()->position);
	m_lineDrawer->AddPosition(s6.lock()->position);
	m_lineDrawer->AddPosition(s7.lock()->position);

	m_pera = std::make_shared<PeraPolygon>(m_dx12);

	m_keyboard = std::make_shared<Keyboard>(m_windowClass.hInstance, m_hWnd);
}

void Application::Run()
{
	Initialize();
	m_dx12->SetBackGroundColor(0.5f, 0.5f, 0.5f);
	float angle = 0.0f;
	float radius = 10.0f;

	MSG msg{};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		m_keyboard->Update();

		if (m_keyboard->GetKey(KeyType::Left))
		{
			angle -= 0.02f;
		}
		if (m_keyboard->GetKey(KeyType::Right))
		{
			angle += 0.02f;
		}

		// DirectX12の処理
		m_dx12->BeginDraw();

		m_dx12->Update();

		// とりあえず三角関数を使って無理やり移動
		m_dx12->SetEyePosition(XMFLOAT3{ std::sin(angle) * radius, 0.0f, -std::cos(angle) * radius });

		m_sphereRenderer->Update();
		m_sphereRenderer->Draw();

		m_lineDrawer->Draw();

		//m_pera->PreDrawPera();
		//m_dx12->SetDepthTexture();
		//m_pera->DrawPera();

		m_dx12->EndDraw();
	}

	Finalize();
}

void Application::Finalize()
{
	UnregisterClass(m_windowClass.lpszClassName, m_windowClass.hInstance);
}
