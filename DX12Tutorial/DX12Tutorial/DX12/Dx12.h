#ifndef DX12_H_
#define DX12_H_

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include <DirectXMath.h>

using namespace Microsoft::WRL;
using namespace DirectX;

class Dx12
{
public:

	Dx12(float windowWidth, float windowHeight, HWND hWnd);
	~Dx12();

	void SetBackGroundColor(float r, float g, float b);
	void ExecuteCommandList();
	void SetRenderTarget();
	void SetViewPort();
	void SetScissorRect();
	void SetEyePosition(const XMFLOAT3& position);

	void BeginDraw();
	void Update();
	void SetDepthTexture();
	void EndDraw();

private:

	void ClearRenderTarget();

	bool CreateDevice();
	bool CreateFactory();
	bool CreateCommandAllocator();
	bool CreateCommandList();
	bool CreateCommandQueue();
	bool CreateSwapChain();
	bool CreateFence();
	bool CreateRTVHeap();
	bool CreateRenderTargetView();
	bool CreateViewPort();
	bool CreateScissorRect();
	bool CreateDepthStencilBuffer();
	bool CreateDSVHeap();
	bool CreateDepthStencilView();
	bool CreateDepthSRVHeap();
	bool CreateDepthShaderResourceView();

public:

	ComPtr<ID3D12Device> Device() const;
	ComPtr<ID3D12GraphicsCommandList> CommandList() const;
	XMMATRIX ViewMat() const;
	XMMATRIX ProjectionMat() const;

private:

	float m_windowWidth;
	float m_windowHeight;
	HWND m_hWnd;
	ComPtr<ID3D12Device> m_device{ nullptr };
	ComPtr<IDXGIFactory6> m_factory{ nullptr };
	ComPtr<IDXGISwapChain4> m_swapChain{ nullptr };
	ComPtr<ID3D12CommandAllocator> m_cmdAllocator{ nullptr };
	ComPtr<ID3D12GraphicsCommandList> m_cmdList{ nullptr };
	ComPtr<ID3D12CommandQueue> m_cmdQueue{ nullptr };
	ComPtr<ID3D12Fence> m_fence{ nullptr };
	UINT64 m_fenceVal{ 0 };
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap{ nullptr };
	std::array<ComPtr<ID3D12Resource>, 2> m_backBuffers;
	UINT m_backBufferIndex;
	D3D12_VIEWPORT m_viewPort{};
	D3D12_RECT m_scissorRect{};
	float m_clearColor[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_eye{ 0.0f, 0.0f, -5.0f };
	XMFLOAT3 m_target{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_up{ 0.0f, 1.0f, 0.0f };
	ComPtr<ID3D12Resource> m_depthBuffer{ nullptr };
	ComPtr<ID3D12DescriptorHeap> m_depthHeap{ nullptr };
	ComPtr<ID3D12DescriptorHeap> m_depthSRVHeap{ nullptr };
};

#endif // !DX12_H_
