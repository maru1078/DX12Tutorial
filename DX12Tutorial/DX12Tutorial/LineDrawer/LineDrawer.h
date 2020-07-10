#ifndef LINE_DRAWER_H_
#define LINE_DRAWER_H_

#include <vector>
#include <memory>
#include <d3d12.h>
#include <wrl.h>
#include <DirectXMath.h>

using namespace Microsoft::WRL;
using namespace DirectX;

class Dx12;

// 線を描画するためのクラス
class LineDrawer
{
public:

	LineDrawer(std::weak_ptr<Dx12> dx12, const XMFLOAT4& color);

	void Draw();

	void AddPosition(const XMFLOAT3& worldPos);

public:

	bool CreateVertexBuffer();
	bool CreateVertexBufferView();

	bool CreateConstantBuffer();
	bool CreateCBVHeap();
	bool CreateConstantBufferView();
	
	bool CreateVertexShader();
	bool CreatePixelShader();
	bool CreateRootSignature();
	bool CreatePipelineState();

private:

	XMFLOAT2 ToScreenPos(const XMFLOAT3& worldPos);
	std::vector<XMFLOAT2> ToScreenPosAll();
	std::vector<XMFLOAT2> SelectVertex(const std::vector<XMFLOAT2>& screenPositions);
	void UpdateVertices();

private:

	std::weak_ptr<Dx12> m_dx12;
	XMFLOAT4 m_color;
	std::vector<XMFLOAT3> m_worldPositions;

	// 頂点関連
	std::vector<XMFLOAT2> m_vertices;
	ComPtr<ID3D12Resource> m_vertexBuffer{ nullptr };
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	// 定数バッファ
	ComPtr<ID3D12Resource> m_constantBuffer{ nullptr };
	// 定数バッファー用ディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> m_cbvHeap{ nullptr };


	// ルートシグネチャ
	ComPtr<ID3D12RootSignature> m_rootSignature{ nullptr };
	// 頂点シェーダー
	ComPtr<ID3DBlob> m_vertexShader{ nullptr };
	// ピクセルシェーダー
	ComPtr<ID3DBlob> m_pixelShader{ nullptr };

	// パイプライン
	ComPtr<ID3D12PipelineState> m_pipeline{ nullptr };
};

#endif // !LINE_DRAWER_H_

