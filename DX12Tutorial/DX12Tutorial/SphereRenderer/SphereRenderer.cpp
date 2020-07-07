#include "SphereRenderer.h"
#include "../DX12/Dx12.h"
#include "../SphereData/SphereData.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

SphereRenderer::SphereRenderer(std::weak_ptr<Dx12> dx12)
	: m_dx12{ dx12 }
{
	if (!CreateVertexBuffer())
	{
		assert(0);
	}
	if (!CreateVertexBufferView())
	{
		assert(0);
	}
	if (!CreateIndexBuffer())
	{
		assert(0);
	}
	if (!CreateIndexBufferView())
	{
		assert(0);
	}
	if (!CreateConstantBuffer())
	{
		assert(0);
	}
	if (!CreateCBVHeap())
	{
		assert(0);
	}
	if (!CreateConstantBufferView())
	{
		assert(0);
	}
	if (!CreateRootSignature())
	{
		assert(0);
	}
	if (!CreateVertexShader())
	{
		assert(0);
	}
	if (!CreatePixelShader())
	{
		assert(0);
	}
	if (!CreatePipelineState())
	{
		assert(0);
	}
}

std::weak_ptr<SphereData> SphereRenderer::CreateSphere(const XMFLOAT3& position, const XMFLOAT4& color, float radius)
{
	auto sphere = std::make_shared<SphereData>(position, color, radius);
	m_spheres.push_back(sphere);
	return sphere;
}

void SphereRenderer::Update()
{
}

void SphereRenderer::Draw()
{
	for (auto sphere : m_spheres)
	{
		// 定数バッファ更新
		ConstantBufferData* mappedData{ nullptr };
		m_constantBuffer->Map(0, nullptr, (void**)&mappedData);
		mappedData->position = sphere->position;
		mappedData->radius = sphere->radius;
		mappedData->color = sphere->color;
		mappedData->world = sphere->World();
		mappedData->view = m_dx12.lock()->ViewMat();
		mappedData->projection = m_dx12.lock()->ProjectionMat();
		m_constantBuffer->Unmap(0, nullptr);

		// レンダーターゲットセット
		m_dx12.lock()->SetRenderTarget();

		// ビューポートセット
		m_dx12.lock()->SetViewPort();

		// シザー矩形セット
		m_dx12.lock()->SetScissorRect();

		// ルートシグネチャセット
		m_dx12.lock()->CommandList()->SetGraphicsRootSignature(m_rootSignature.Get());

		// パイプラインセット
		m_dx12.lock()->CommandList()->SetPipelineState(m_pipelineState.Get());

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

		// コマンドリスト実行
		m_dx12.lock()->ExecuteCommandList();
	}
}

bool SphereRenderer::CreateVertexBuffer()
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

bool SphereRenderer::CreateVertexBufferView()
{
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = m_vertexBuffer->GetDesc().Width;
	m_vertexBufferView.StrideInBytes = sizeof(XMFLOAT3);

	return true;
}

bool SphereRenderer::CreateIndexBuffer()
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

bool SphereRenderer::CreateIndexBufferView()
{
	// インデックスバッファービュー作成
	m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	m_indexBufferView.SizeInBytes = m_indexBuffer->GetDesc().Width;

	return true;
}

bool SphereRenderer::CreateConstantBuffer()
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

	return true;
}

bool SphereRenderer::CreateCBVHeap()
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

bool SphereRenderer::CreateConstantBufferView()
{
	// 定数バッファービュー作成
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = m_constantBuffer->GetDesc().Width;
	m_dx12.lock()->Device()->CreateConstantBufferView(
		&cbvDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());

	return true;
}

bool SphereRenderer::CreateRootSignature()
{
	// ルートシグネチャ作成
	D3D12_DESCRIPTOR_RANGE range{};
	range.NumDescriptors = 1;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	range.BaseShaderRegister = 0;
	range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootParam{};
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParam.DescriptorTable.NumDescriptorRanges = 1;
	rootParam.DescriptorTable.pDescriptorRanges = &range;

	D3D12_ROOT_SIGNATURE_DESC rsDesc{};
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rsDesc.NumParameters = 1;
	rsDesc.pParameters = &rootParam;

	ID3DBlob* rsBlob{ nullptr };
	auto result = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rsBlob,
		nullptr);

	if (FAILED(result)) return false;

	result = m_dx12.lock()->Device()->CreateRootSignature(
		0,
		rsBlob->GetBufferPointer(),
		rsBlob->GetBufferSize(),
		IID_PPV_ARGS(m_rootSignature.ReleaseAndGetAddressOf()));

	rsBlob->Release();

	if (FAILED(result)) return false;

	return true;
}

bool SphereRenderer::CreateVertexShader()
{
	// シェーダー作成
	auto result = D3DCompileFromFile(
		L"Shader/SphereVertexShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		m_vertexShader.ReleaseAndGetAddressOf(),
		nullptr);

	if (FAILED(result)) return false;

	return true;
}

bool SphereRenderer::CreatePixelShader()
{
	auto result = D3DCompileFromFile(
		L"Shader/SpherePixelShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		m_pixelShader.ReleaseAndGetAddressOf(),
		nullptr);

	if (FAILED(result)) return false;

	return true;
}

bool SphereRenderer::CreatePipelineState()
{
	// インプットレイアウト作成
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_RENDER_TARGET_BLEND_DESC blendDesc{};
	blendDesc.BlendEnable = false;
	blendDesc.LogicOpEnable = false;
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// パイプライン作成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC plsDesc{};
	plsDesc.pRootSignature = m_rootSignature.Get();

	plsDesc.VS.pShaderBytecode = m_vertexShader->GetBufferPointer();
	plsDesc.VS.BytecodeLength = m_vertexShader->GetBufferSize();

	plsDesc.PS.pShaderBytecode = m_pixelShader->GetBufferPointer();
	plsDesc.PS.BytecodeLength = m_pixelShader->GetBufferSize();

	plsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	plsDesc.RasterizerState.MultisampleEnable = false;
	plsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	plsDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	plsDesc.RasterizerState.DepthClipEnable = true;

	plsDesc.BlendState.AlphaToCoverageEnable = false;
	plsDesc.BlendState.IndependentBlendEnable = false;
	plsDesc.BlendState.RenderTarget[0] = blendDesc;

	plsDesc.InputLayout.pInputElementDescs = inputLayout;
	plsDesc.InputLayout.NumElements = _countof(inputLayout);

	plsDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

	plsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	plsDesc.NumRenderTargets = 1;
	plsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	plsDesc.SampleDesc.Count = 1;
	plsDesc.SampleDesc.Quality = 0;

	plsDesc.DepthStencilState.StencilEnable = false;
	plsDesc.DepthStencilState.DepthEnable = true;
	plsDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	plsDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	plsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	auto result = m_dx12.lock()->Device()->CreateGraphicsPipelineState(
		&plsDesc, IID_PPV_ARGS(m_pipelineState.ReleaseAndGetAddressOf()));

	if (FAILED(result)) return false;

	return true;
}
