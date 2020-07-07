#ifndef SPHERE_RENDERER_H_
#define SPHERE_RENDERER_H_

#include <memory>
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <DirectXMath.h>

using namespace Microsoft::WRL;
using namespace DirectX;

class Dx12;
class SphereData;

// 球体の描画を行うクラス
class SphereRenderer
{
public:

	SphereRenderer(std::weak_ptr<Dx12> dx12);

	std::weak_ptr<SphereData> CreateSphere(const XMFLOAT3& position, const XMFLOAT4& color, float radius);

	void Update();
	void Draw();

public:

	bool CreateVertexBuffer();
	bool CreateVertexBufferView();
	bool CreateIndexBuffer();
	bool CreateIndexBufferView();
	bool CreateConstantBuffer();
	bool CreateCBVHeap();
	bool CreateConstantBufferView();
	bool CreateRootSignature();
	bool CreateVertexShader();
	bool CreatePixelShader();
	bool CreatePipelineState();

private:

	std::weak_ptr<Dx12> m_dx12;
	std::vector<std::shared_ptr<SphereData>> m_spheres;

	const int m_uMax{ 30 };
	const int m_vMax{ 15 };

	unsigned int m_indexNum;

	// 頂点バッファー
	ComPtr<ID3D12Resource> m_vertexBuffer{ nullptr };
	// 頂点バッファービュー
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};
	// インデックスバッファー
	ComPtr<ID3D12Resource> m_indexBuffer{ nullptr };
	// インデックスバッファービュー
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView{};
	// 定数バッファ
	ComPtr<ID3D12Resource> m_constantBuffer{ nullptr };
	// 定数バッファー用ディスクリプターヒープ
	ComPtr<ID3D12DescriptorHeap> m_cbvHeap{ nullptr };
	// ルートシグネチャ
	ComPtr<ID3D12RootSignature> m_rootSignature{ nullptr };
	// 頂点シェーダー
	ComPtr<ID3DBlob> m_vertexShader{ nullptr };
	// ピクセルシェーダー
	ComPtr<ID3DBlob> m_pixelShader{ nullptr };
	// パイプライン
	ComPtr<ID3D12PipelineState> m_pipelineState{ nullptr };

	struct ConstantBufferData
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMFLOAT3 position;
		float radius;
		XMFLOAT4 color;
	};
};

#endif // !SPHERE_RENDERER_H_

