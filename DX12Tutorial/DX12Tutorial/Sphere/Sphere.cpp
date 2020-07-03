#include "Sphere.h"
#include "../DX12/Dx12.h"
#include <cmath>

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

Sphere::Sphere(std::weak_ptr<Dx12> dx12, XMFLOAT3 position, XMFLOAT4 color, float radius)
	: m_dx12{ dx12 }
	, m_position{ position }
	, m_radius{ radius }
{
	const int uMax = 30;
	const int vMax = 15;

	m_vertexNum = uMax * (vMax + 1);
	m_indexNum = 2 * vMax * (uMax + 1);

	m_vertices.resize(m_vertexNum);
	m_indices.resize(m_indexNum);

	// ���_�f�[�^�쐬
	for (int v = 0; v <= vMax; v++)
	{
		for (int u = 0; u < uMax; u++)
		{
			float theta = XMConvertToRadians(180.0f * v / vMax);
			float phi = XMConvertToRadians(360.0f * u / uMax);

			float x = std::sin(theta) * std::cos(phi) * m_radius;
			float y = std::cos(theta) * m_radius;
			float z = std::sin(theta) * std::sin(phi) * m_radius;
			m_vertices[uMax * v + u] = XMFLOAT3(x, y, z);
		}
	}

	// �C���f�b�N�X�f�[�^�쐬
	int i = 0;
	for (int v = 0; v < vMax; v++)
	{
		for (int u = 0; u <= uMax; u++)
		{
			if (u == uMax)
			{
				m_indices[i] = v * uMax;
				i++;
				m_indices[i] = (v + 1) * uMax;
				i++;
			}
			else
			{
				m_indices[i] = (v * uMax) + u;
				i++;
				m_indices[i] = m_indices[i - 1] + uMax;
				i++;
			}
		}
	}

	// ���ꂼ��̃o�b�t�@�[�����
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
	resDesc.Width = sizeof(m_vertices[0]) * m_vertices.size();

	sizeof(XMFLOAT3);

	// ���_�o�b�t�@�쐬
	m_dx12.lock()->Device()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_vertexBuffer.ReleaseAndGetAddressOf()));

	XMFLOAT3* vertMap{ nullptr };
	m_vertexBuffer->Map(0, nullptr, (void**)&vertMap);
	std::copy(m_vertices.begin(), m_vertices.end(), vertMap);
	m_vertexBuffer->Unmap(0, nullptr);
	vertMap = nullptr;

	// ���_�o�b�t�@�[�r���[�쐬
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = m_vertexBuffer->GetDesc().Width;
	m_vertexBufferView.StrideInBytes = sizeof(m_vertices[0]);

	// �C���f�b�N�X�o�b�t�@�쐬
	resDesc.Width = sizeof(m_indices[0]) * m_indices.size();
	m_dx12.lock()->Device()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_indexBuffer.ReleaseAndGetAddressOf()));

	unsigned int* indMap{ nullptr };
	m_indexBuffer->Map(0, nullptr, (void**)&indMap);
	std::copy(m_indices.begin(), m_indices.end(), indMap);
	m_indexBuffer->Unmap(0, nullptr);
	indMap = nullptr;

	// �C���f�b�N�X�o�b�t�@�[�r���[�쐬
	m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = m_indexBuffer->GetDesc().Width;

	// �萔�o�b�t�@�쐬
	resDesc.Width = (sizeof(*m_cbData) + 0xff) & ~0xff;
	m_dx12.lock()->Device()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_constantBuffer.ReleaseAndGetAddressOf()));

	m_constantBuffer->Map(0, nullptr, (void**)&m_cbData);
	m_cbData->position = XMFLOAT4{ position.x, position.y, position.z, 0.0f };
	m_cbData->color = color;
	m_cbData->world = m_dx12.lock()->WorldMat() * XMMatrixTranslation(position.x, position.y, position.z);
	m_cbData->view = m_dx12.lock()->ViewMat();
	m_cbData->projection = m_dx12.lock()->ProjectionMat();
	//m_constantBuffer->Unmap(0, nullptr);
	//m_cbData = nullptr;

	// �萔�o�b�t�@�r���[�̂��߂̃q�[�v�쐬
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	m_dx12.lock()->Device()->CreateDescriptorHeap(
		&heapDesc, IID_PPV_ARGS(m_cbvHeap.ReleaseAndGetAddressOf()));

	// �萔�o�b�t�@�[�r���[�쐬
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = m_constantBuffer->GetDesc().Width;
	m_dx12.lock()->Device()->CreateConstantBufferView(
		&cbvDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());

	// ���[�g�V�O�l�`���쐬
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

	result = m_dx12.lock()->Device()->CreateRootSignature(
		0,
		rsBlob->GetBufferPointer(),
		rsBlob->GetBufferSize(),
		IID_PPV_ARGS(m_rootSignature.ReleaseAndGetAddressOf()));

	rsBlob->Release();

	// �V�F�[�_�[�쐬
	result = D3DCompileFromFile(
		L"Shader/SphereVertexShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		m_vertexShader.ReleaseAndGetAddressOf(),
		nullptr);

	result = D3DCompileFromFile(
		L"Shader/SpherePixelShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		m_pixelShader.ReleaseAndGetAddressOf(),
		nullptr);

	// �C���v�b�g���C�A�E�g�쐬
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = 
	{
		{ "INPOSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_RENDER_TARGET_BLEND_DESC blendDesc{};
	blendDesc.BlendEnable = false;
	blendDesc.LogicOpEnable = false;
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// �p�C�v���C���쐬
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

	result = m_dx12.lock()->Device()->CreateGraphicsPipelineState(
		&plsDesc, IID_PPV_ARGS(m_pipelineState.ReleaseAndGetAddressOf()));
}

Sphere::~Sphere()
{
}

void Sphere::Update()
{
	m_angle += 0.01f;
	// �s��̏����X�V
	m_cbData->world = XMMatrixTranslation(m_position.x, m_position.y, m_position.z) * XMMatrixRotationY(m_angle);
}

void Sphere::Draw()
{
	// ���[�g�V�O�l�`���Z�b�g
	m_dx12.lock()->CommandList()->SetGraphicsRootSignature(m_rootSignature.Get());

	// �p�C�v���C���Z�b�g
	m_dx12.lock()->CommandList()->SetPipelineState(m_pipelineState.Get());

	// ���_�o�b�t�@�[�r���[�Z�b�g
	m_dx12.lock()->CommandList()->IASetVertexBuffers(0, 1, &m_vertexBufferView);

	// �C���f�b�N�X�o�b�t�@�[�r���[�Z�b�g
	m_dx12.lock()->CommandList()->IASetIndexBuffer(&m_indexBufferView);
	
	// �q�[�v�Z�b�g
	m_dx12.lock()->CommandList()->SetDescriptorHeaps(1, m_cbvHeap.GetAddressOf());
	
	// �q�[�v�ƃ��[�g�p�����[�^�[�̊֘A�t��
	m_dx12.lock()->CommandList()->SetGraphicsRootDescriptorTable(
		0,
		m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
	
	// �v���~�e�B�u�g�|���W�[�Z�b�g
	m_dx12.lock()->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	// �h���[�R�[��
	m_dx12.lock()->CommandList()->DrawIndexedInstanced(m_indexNum, 1, 0, 0, 0);
}