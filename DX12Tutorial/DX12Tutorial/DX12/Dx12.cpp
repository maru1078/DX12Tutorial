#include "Dx12.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

void EnableDebugLayer()
{
#ifdef _DEBUG

	ID3D12Debug* debugLayer{ nullptr };
	auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

	if (FAILED(result)) return;

	debugLayer->EnableDebugLayer();
	debugLayer->Release();

#endif // _DEBUG

}

Dx12::Dx12(float windowWidth, float windowHeight, HWND hWnd)
	: m_windowWidth{ windowWidth }
	, m_windowHeight{ windowHeight }
	, m_hWnd{ hWnd }
{
	// デバッグレイヤーをオンに
	EnableDebugLayer();

	if (!CreateDevice())
	{
		assert(0);
	}
	if (!CreateFactory())
	{
		assert(0);
	}
	if (!CreateCommandAllocator())
	{
		assert(0);
	}
	if (!CreateCommandList())
	{
		assert(0);
	}
	if (!CreateCommandQueue())
	{
		assert(0);
	}
	if (!CreateSwapChain())
	{
		assert(0);
	}
	if (!CreateFence())
	{
		assert(0);
	}
	if (!CreateRTVHeap())
	{
		assert(0);
	}
	if (!CreateRenderTargetView())
	{
		assert(0);
	}
	if (!CreateViewPort())
	{
		assert(0);
	}
	if (!CreateScissorRect())
	{
		assert(0);
	}
	if (!CreateDepthStencilBuffer())
	{
		assert(0);
	}
	if (!CreateDSVHeap())
	{
		assert(0);
	}
	if (!CreateDepthStencilView())
	{
		assert(0);
	}
	if (!CreateDepthSRVHeap())
	{
		assert(0);
	}
	if (!CreateDepthShaderResourceView())
	{
		assert(0);
	}
}

Dx12::~Dx12()
{
}

void Dx12::SetBackGroundColor(float r, float g, float b)
{
	m_clearColor[0] = r;
	m_clearColor[1] = g;
	m_clearColor[2] = b;
}

void Dx12::ExecuteCommandList()
{
	m_cmdList->Close();

	ID3D12CommandList* cmdLists[]{ m_cmdList.Get() };
	m_cmdQueue->ExecuteCommandLists(1, cmdLists);

	m_cmdQueue->Signal(m_fence.Get(), ++m_fenceVal);
	while (m_fence->GetCompletedValue() != m_fenceVal)
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);
		m_fence->SetEventOnCompletion(m_fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	m_cmdAllocator->Reset();
	m_cmdList->Reset(m_cmdAllocator.Get(), nullptr);
}

void Dx12::SetRenderTarget()
{
	auto rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += m_backBufferIndex
		* m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	auto dsvHandle = m_depthHeap->GetCPUDescriptorHandleForHeapStart();

	m_cmdList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);
}

void Dx12::SetViewPort()
{
	// ビューポートセット
	m_cmdList->RSSetViewports(1, &m_viewPort);
}

void Dx12::SetScissorRect()
{
	// シザー矩形セット
	m_cmdList->RSSetScissorRects(1, &m_scissorRect);
}

void Dx12::SetEyePosition(const XMFLOAT3 & position)
{
	m_eye = position;
}

void Dx12::BeginDraw()
{
	m_backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_backBuffers.at(m_backBufferIndex).Get();
	barrier.Transition.Subresource = 0;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_cmdList->ResourceBarrier(1, &barrier);

	SetRenderTarget();

	ClearRenderTarget();

	SetViewPort();

	SetScissorRect();

}

void Dx12::Update()
{

}

void Dx12::SetDepthTexture()
{
	m_cmdList->SetDescriptorHeaps(1, m_depthSRVHeap.GetAddressOf());
	m_cmdList->SetGraphicsRootDescriptorTable(0, m_depthSRVHeap->GetGPUDescriptorHandleForHeapStart());
}

void Dx12::EndDraw()
{
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_backBuffers.at(m_backBufferIndex).Get();
	barrier.Transition.Subresource = 0;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_cmdList->ResourceBarrier(1, &barrier);

	ExecuteCommandList();
	m_swapChain->Present(1, 0);
}

void Dx12::ClearRenderTarget()
{
	auto rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += m_backBufferIndex
		* m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	m_cmdList->ClearRenderTargetView(rtvHandle, m_clearColor, 0, nullptr);

	auto dsvHandle = m_depthHeap->GetCPUDescriptorHandleForHeapStart();

	m_cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

bool Dx12::CreateDevice()
{
	auto result = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_12_1,
		IID_PPV_ARGS(m_device.ReleaseAndGetAddressOf()));

	if (FAILED(result)) return false;

	return true;
}

bool Dx12::CreateFactory()
{
	// ファクトリー作成
#ifdef _DEBUG

	auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG,
		IID_PPV_ARGS(m_factory.ReleaseAndGetAddressOf()));

#else

	auto result = CreateDXGIFactory1(IID_PPV_ARGS(m_factory.ReleaseAndGetAddressOf()));

#endif // _DEBUG

	if (FAILED(result)) return false;

	return true;
}

bool Dx12::CreateCommandAllocator()
{
	// コマンドアロケーター作成
	auto result = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(m_cmdAllocator.ReleaseAndGetAddressOf()));

	if (FAILED(result)) return false;

	return true;
}

bool Dx12::CreateCommandList()
{
	// コマンドリスト作成
	auto result = m_device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_cmdAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(m_cmdList.ReleaseAndGetAddressOf()));

	if (FAILED(result)) return false;

	return true;
}

bool Dx12::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0;
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	auto result = m_device->CreateCommandQueue(&queueDesc,
		IID_PPV_ARGS(m_cmdQueue.ReleaseAndGetAddressOf()));

	if (FAILED(result)) return false;

	return true;
}

bool Dx12::CreateSwapChain()
{
	// スワップチェーン作成
	DXGI_SWAP_CHAIN_DESC1 scDesc{};
	scDesc.Width = m_windowWidth;
	scDesc.Height = m_windowHeight;
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.Stereo = false;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	scDesc.BufferCount = 2;
	scDesc.Scaling = DXGI_SCALING_STRETCH;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	auto result = m_factory->CreateSwapChainForHwnd(
		m_cmdQueue.Get(),
		m_hWnd,
		&scDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)m_swapChain.ReleaseAndGetAddressOf());

	if (FAILED(result)) return false;

	return true;
}

bool Dx12::CreateFence()
{
	// フェンスの作成
	auto result = m_device->CreateFence(m_fenceVal, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(m_fence.ReleaseAndGetAddressOf()));

	if (FAILED(result)) return false;

	return true;
}

bool Dx12::CreateRTVHeap()
{
	// ヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;
	auto result = m_device->CreateDescriptorHeap(&heapDesc,
		IID_PPV_ARGS(m_rtvHeap.ReleaseAndGetAddressOf()));

	if (FAILED(result)) return false;

	return true;
}

bool Dx12::CreateRenderTargetView()
{
	// レンダーターゲットビュー作成
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	auto handle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

	for (int i = 0; i < 2; ++i)
	{
		auto result = m_swapChain->GetBuffer(
			i,
			IID_PPV_ARGS(m_backBuffers.at(i).ReleaseAndGetAddressOf()));

		if (FAILED(result)) return false;

		m_device->CreateRenderTargetView(
			m_backBuffers.at(i).Get(),
			&rtvDesc,
			handle);

		handle.ptr += m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	return true;
}

bool Dx12::CreateViewPort()
{
	// 通常のビューポート作成
	m_viewPort.TopLeftX = 0;
	m_viewPort.TopLeftY = 0;
	m_viewPort.Width = m_windowWidth;
	m_viewPort.Height = m_windowHeight;
	m_viewPort.MaxDepth = 1.0f;
	m_viewPort.MinDepth = 0.0f;

	return true;
}

bool Dx12::CreateScissorRect()
{
	// 通常のシザー矩形作成
	m_scissorRect.left = 0;
	m_scissorRect.top = 0;
	m_scissorRect.right = m_windowWidth;
	m_scissorRect.bottom = m_windowHeight;

	return true;
}

bool Dx12::CreateDepthStencilBuffer()
{
	// バッファー作成
	D3D12_RESOURCE_DESC depthResDesc{};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = m_windowWidth;
	depthResDesc.Height = m_windowHeight;
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	auto result = m_device->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(m_depthBuffer.ReleaseAndGetAddressOf()));

	if (FAILED(result)) return false;

	return true;
}

bool Dx12::CreateDSVHeap()
{
	// ヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC depthHeapDesc{};
	depthHeapDesc.NumDescriptors = 1;
	depthHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	auto result = m_device->CreateDescriptorHeap(
		&depthHeapDesc, IID_PPV_ARGS(m_depthHeap.ReleaseAndGetAddressOf()));

	if (FAILED(result)) return false;

	return true;
}

bool Dx12::CreateDepthStencilView()
{
	// ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	m_device->CreateDepthStencilView(
		m_depthBuffer.Get(),
		&dsvDesc,
		m_depthHeap->GetCPUDescriptorHandleForHeapStart());

	return true;
}

bool Dx12::CreateDepthSRVHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 1;
	auto result = m_device->CreateDescriptorHeap(&heapDesc,
		IID_PPV_ARGS(m_depthSRVHeap.ReleaseAndGetAddressOf()));

	return true;
}

bool Dx12::CreateDepthShaderResourceView()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	m_device->CreateShaderResourceView(m_depthBuffer.Get(),
		&srvDesc, m_depthSRVHeap->GetCPUDescriptorHandleForHeapStart());

	return true;
}

ComPtr<ID3D12Device> Dx12::Device() const
{
	return m_device;
}

ComPtr<ID3D12GraphicsCommandList> Dx12::CommandList() const
{
	return m_cmdList;
}

XMMATRIX Dx12::ViewMat() const
{
	return XMMatrixLookAtLH(XMLoadFloat3(&m_eye), XMLoadFloat3(&m_target), XMLoadFloat3(&m_up));
}

XMMATRIX Dx12::ProjectionMat() const
{
	return XMMatrixPerspectiveFovLH(
		XM_PIDIV4,
		m_windowWidth / m_windowHeight,
		0.1f,
		1000.0f);
}
