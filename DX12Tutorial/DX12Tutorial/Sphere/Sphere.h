#ifndef SPHERE_H_
#define SPHERE_H_

#include <memory>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include <DirectXMath.h>

using namespace Microsoft::WRL;
using namespace DirectX;

class Dx12;

class Sphere
{
public:

	Sphere(std::weak_ptr<Dx12> dx12, XMFLOAT3 position, XMFLOAT4 color, float radius);
	~Sphere();

public:

	void Update();
	void Draw();

public:

	XMFLOAT3 Position() const;
	XMFLOAT4 Color() const;
	float Radius() const;
	XMMATRIX World();

private:

	// ↓いったん保留↓

	bool CreateVertexBuffer();
	bool CreateVertexBufferView();
	bool CreateIndexBuffer();
	bool CreateIndexBufferView();
	bool CreateConstantBuffer();
	bool CreateCBVHeap();
	bool CreateConstantBufferView();


private:

	std::weak_ptr<Dx12> m_dx12;

	const int m_uMax{ 30 };
	const int m_vMax{ 15 };

	XMFLOAT3 m_position;
	float m_radius;
	XMFLOAT4 m_color;
	float m_angle{ 0.0f };

	// ↓いったん保留↓

	// インデックス情報
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

#endif // !SPHERE_H_

