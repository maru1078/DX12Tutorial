#include "Sphere.h"
#include "../DX12/Dx12.h"
#include <cmath>

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

Sphere::Sphere(std::weak_ptr<Dx12> dx12, XMFLOAT3 position, XMFLOAT4 color, float radius)
	: m_dx12{ dx12 }
	, m_position{ position }
	, m_color{ color }
	, m_radius{ radius }
{
	//if (!CreateVertexBuffer())
	//{
	//	assert(0);
	//}
	//if (!CreateVertexBufferView())
	//{
	//	assert(0);
	//}
	//if (!CreateIndexBuffer())
	//{
	//	assert(0);
	//}
	//if (!CreateIndexBufferView())
	//{
	//	assert(0);
	//}
	//if (!CreateConstantBuffer())
	//{
	//	assert(0);
	//}
	//if (!CreateCBVHeap())
	//{
	//	assert(0);
	//}
	//if (!CreateConstantBufferView())
	//{
	//	assert(0);
	//}
}

Sphere::~Sphere()
{
}

void Sphere::Update()
{
	m_angle += 0.01f;

	//// 定数バッファ更新
	//// こんな感じでいいのかな・・・？
	//ConstantBufferData* cbData{ nullptr };
	//m_constantBuffer->Map(0, nullptr, (void**)&cbData);
	//cbData->position = m_position;
	//cbData->radius = m_radius;
	//cbData->color = m_color;
	//cbData->world = XMMatrixTranslation(m_position.x, m_position.y, m_position.z) * XMMatrixRotationY(m_angle);
	//cbData->view = m_dx12.lock()->ViewMat();
	//cbData->projection = m_dx12.lock()->ProjectionMat();
	//m_constantBuffer->Unmap(0, nullptr);
}

void Sphere::Draw()
{
	// 頂点バッファービューセット
	m_dx12.lock()->CommandList()->IASetVertexBuffers(0, 1, &m_vertexBufferView);

	// インデックスバッファービューセット
	m_dx12.lock()->CommandList()->IASetIndexBuffer(&m_indexBufferView);
	
	// ヒープセット
	m_dx12.lock()->CommandList()->SetDescriptorHeaps(1, m_cbvHeap.GetAddressOf());
	
	// ヒープとルートパラメーターの関連付け
	m_dx12.lock()->CommandList()->SetGraphicsRootDescriptorTable(
		0,
		m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
	
	// プリミティブトポロジーセット
	m_dx12.lock()->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	// ドローコール
	m_dx12.lock()->CommandList()->DrawIndexedInstanced(m_indexNum, 1, 0, 0, 0);
}

XMFLOAT3 Sphere::Position() const
{
	return m_position;
}

XMFLOAT4 Sphere::Color() const
{
	return m_color;
}

float Sphere::Radius() const
{
	return m_radius;
}

XMMATRIX Sphere::World()
{
	return XMMatrixTranslation(m_position.x, m_position.y, m_position.z) * XMMatrixRotationY(m_angle);
}

// ↓いったん保留↓

bool Sphere::CreateVertexBuffer()
{
	unsigned int vertexNum;
	vertexNum = m_uMax * (m_vMax + 1);
	std::vector<XMFLOAT3> vertices(vertexNum);

	// 頂点データ作成
	for (int v = 0; v <= m_vMax; v++)
	{
		for (int u = 0; u < m_uMax; u++)
		{
			float theta = XMConvertToRadians(180.0f * v / m_vMax);
			float phi = XMConvertToRadians(360.0f * u / m_uMax);

			float x = std::sin(theta) * std::cos(phi);
			float y = std::cos(theta);
			float z = std::sin(theta) * std::sin(phi);
			vertices[m_uMax * v + u] = XMFLOAT3(x, y, z);
		}
	}

	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resDesc{};
	resDesc.DepthOrArraySize = 1;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.Height = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Width = sizeof(vertices[0]) * vertices.size();

	// 頂点バッファ作成
	auto result = m_dx12.lock()->Device()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_vertexBuffer.ReleaseAndGetAddressOf()));

	if (FAILED(result)) return false;

	XMFLOAT3* vertMap{ nullptr };
	result = m_vertexBuffer->Map(0, nullptr, (void**)&vertMap);
	
	if (FAILED(result)) return false;
	
	std::copy(vertices.begin(), vertices.end(), vertMap);
	m_vertexBuffer->Unmap(0, nullptr);

	return true;
}

bool Sphere::CreateVertexBufferView()
{
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = m_vertexBuffer->GetDesc().Width;
	m_vertexBufferView.StrideInBytes = sizeof(XMFLOAT3);

	return true;
}

bool Sphere::CreateIndexBuffer()
{
	m_indexNum = 2 * m_vMax * (m_uMax + 1);
	std::vector<unsigned short> indices(m_indexNum);

	// インデックスデータ作成
	int i = 0;
	for (int v = 0; v < m_vMax; v++)
	{
		for (int u = 0; u <= m_uMax; u++)
		{
			if (u == m_uMax)
			{
				indices[i] = v * m_uMax;
				i++;
				indices[i] = (v + 1) * m_uMax;
				i++;
			}
			else
			{
				indices[i] = (v * m_uMax) + u;
				i++;
				indices[i] = indices[i - 1] + m_uMax;
				i++;
			}
		}
	}

	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resDesc{};
	resDesc.DepthOrArraySize = 1;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.Height = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Width = sizeof(indices[0]) * indices.size();

	// インデックスバッファ作成
	auto result = m_dx12.lock()->Device()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_indexBuffer.ReleaseAndGetAddressOf()));

	if (FAILED(result)) return false;

	unsigned short* indMap{ nullptr };
	result = m_indexBuffer->Map(0, nullptr, (void**)&indMap);
	
	if (FAILED(result)) return false;
	
	std::copy(indices.begin(), indices.end(), indMap);
	m_indexBuffer->Unmap(0, nullptr);

	return true;
}

bool Sphere::CreateIndexBufferView()
{
	// インデックスバッファービュー作成
	m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	m_indexBufferView.SizeInBytes = m_indexBuffer->GetDesc().Width;

	return true;
}

bool Sphere::CreateConstantBuffer()
{
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resDesc{};
	resDesc.DepthOrArraySize = 1;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.Height = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Width = (sizeof(ConstantBufferData) + 0xff) & ~0xff;

	// 定数バッファ作成
	auto result = m_dx12.lock()->Device()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_constantBuffer.ReleaseAndGetAddressOf()));

	if (FAILED(result)) return false;

	ConstantBufferData* mappedData{ nullptr };
	m_constantBuffer->Map(0, nullptr, (void**)&mappedData);
	mappedData->position = m_position;
	mappedData->radius = m_radius;
	mappedData->color = m_color;
	mappedData->world = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	mappedData->view = m_dx12.lock()->ViewMat();
	mappedData->projection = m_dx12.lock()->ProjectionMat();
	m_constantBuffer->Unmap(0, nullptr);

	return true;
}

bool Sphere::CreateCBVHeap()
{
	// 定数バッファビューのためのヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	auto result = m_dx12.lock()->Device()->CreateDescriptorHeap(
		&heapDesc, IID_PPV_ARGS(m_cbvHeap.ReleaseAndGetAddressOf()));

	if (FAILED(result)) return false;

	return true;
}

bool Sphere::CreateConstantBufferView()
{
	// 定数バッファービュー作成
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = m_constantBuffer->GetDesc().Width;
	m_dx12.lock()->Device()->CreateConstantBufferView(
		&cbvDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());

	return true;
}
