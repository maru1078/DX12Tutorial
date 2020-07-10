#include "LineDrawer.h"
#include "../DX12/Dx12.h"

#include <d3dcompiler.h>

LineDrawer::LineDrawer(std::weak_ptr<Dx12> dx12, const XMFLOAT4& color)
	: m_dx12{ dx12 }
	, m_color{ color }
{
	if (!CreateVertexBuffer())
	{
		assert(0);
	}
	if (!CreateVertexBufferView())
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

void LineDrawer::Draw()
{
	// ���_���W���X�V�i�ǂ̒��_���g�������ς���Ă���j
	UpdateVertices();

	// �u���_2��I��ŕ`��v���J��Ԃ�
	for (int i = 0; i < m_vertices.size() - 1; ++i)
	{
		// ���_�o�b�t�@�[�̍X�V
		XMFLOAT2* vertMap{ nullptr };
		m_vertexBuffer->Map(0, nullptr, (void**)&vertMap);
		std::copy(m_vertices.begin() + i, m_vertices.begin() + i + 2, vertMap);
		m_vertexBuffer->Unmap(0, nullptr);

		// �����_�[�^�[�Q�b�g�Z�b�g
		m_dx12.lock()->SetRenderTarget();

		// �r���[�|�[�g�Z�b�g
		m_dx12.lock()->SetViewPort();

		// �V�U�[��`�Z�b�g
		m_dx12.lock()->SetScissorRect();

		// ���[�g�V�O�l�`���Z�b�g
		m_dx12.lock()->CommandList()->SetGraphicsRootSignature(m_rootSignature.Get());

		// �p�C�v���C���Z�b�g
		m_dx12.lock()->CommandList()->SetPipelineState(m_pipeline.Get());

		// ���_�o�b�t�@�[�r���[�Z�b�g
		m_dx12.lock()->CommandList()->IASetVertexBuffers(0, 1, &m_vertexBufferView);

		// �q�[�v�Z�b�g
		m_dx12.lock()->CommandList()->SetDescriptorHeaps(1, m_cbvHeap.GetAddressOf());

		// �q�[�v�ƃ��[�g�p�����[�^�[�̊֘A�t��
		m_dx12.lock()->CommandList()->SetGraphicsRootDescriptorTable(
			0,
			m_cbvHeap->GetGPUDescriptorHandleForHeapStart());

		// �v���~�e�B�u�g�|���W�[�Z�b�g
		m_dx12.lock()->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		// �h���[�R�[��
		m_dx12.lock()->CommandList()->DrawInstanced(2, 1, 0, 0);

		// �R�}���h���X�g���s
		m_dx12.lock()->ExecuteCommandList();
	}
}

void LineDrawer::AddPosition(const XMFLOAT3 & worldPos)
{
	m_worldPositions.push_back(worldPos);
}

bool LineDrawer::CreateVertexBuffer()
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
	resDesc.Width = sizeof(XMFLOAT2) * 2; // ���_2��
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;

	// �o�b�t�@�[�쐬
	auto result = m_dx12.lock()->Device()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_vertexBuffer.ReleaseAndGetAddressOf()));

	if (FAILED(result)) return false;

	return true;
}

bool LineDrawer::CreateVertexBufferView()
{
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = m_vertexBuffer->GetDesc().Width;
	m_vertexBufferView.StrideInBytes = sizeof(XMFLOAT2);

	return true;
}

bool LineDrawer::CreateConstantBuffer()
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
	resDesc.Width = (sizeof(XMFLOAT4) + 0xff) & ~0xff;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;

	// �o�b�t�@�[�쐬
	auto result = m_dx12.lock()->Device()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_constantBuffer.ReleaseAndGetAddressOf()));

	if (FAILED(result)) return false;

	XMFLOAT4* colMap{ nullptr };
	result = m_constantBuffer->Map(0, nullptr, (void**)&colMap);

	if (FAILED(result)) return false;

	*colMap = m_color;
	m_constantBuffer->Unmap(0, nullptr);

	return true;
}

bool LineDrawer::CreateCBVHeap()
{
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

bool LineDrawer::CreateConstantBufferView()
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = m_constantBuffer->GetDesc().Width;
	m_dx12.lock()->Device()->CreateConstantBufferView(
		&cbvDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());

	return true;
}

bool LineDrawer::CreateVertexShader()
{
	auto result = D3DCompileFromFile(
		L"Shader/LineVertexShader.hlsl",
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

bool LineDrawer::CreatePixelShader()
{
	auto result = D3DCompileFromFile(
		L"Shader/LinePixelShader.hlsl",
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

bool LineDrawer::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE range{};
	range.BaseShaderRegister = 0;
	range.NumDescriptors = 1;
	range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

	D3D12_ROOT_PARAMETER rp{};
	rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rp.DescriptorTable.NumDescriptorRanges = 1;
	rp.DescriptorTable.pDescriptorRanges = &range;

	D3D12_ROOT_SIGNATURE_DESC rsDesc{};
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rsDesc.NumParameters = 1;
	rsDesc.pParameters = &rp;

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

bool LineDrawer::CreatePipelineState()
{
	// �C���v�b�g���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
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

	plsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	plsDesc.NumRenderTargets = 1;
	plsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	plsDesc.SampleDesc.Count = 1;
	plsDesc.SampleDesc.Quality = 0;

	plsDesc.DepthStencilState.StencilEnable = false;
	plsDesc.DepthStencilState.DepthEnable = false;

	auto result = m_dx12.lock()->Device()->CreateGraphicsPipelineState(
		&plsDesc, IID_PPV_ARGS(m_pipeline.ReleaseAndGetAddressOf()));

	if (FAILED(result)) return false;

	return true;
}

XMFLOAT2 LineDrawer::ToScreenPos(const XMFLOAT3 & worldPos)
{
	auto view = m_dx12.lock()->ViewMat();
	auto projection = m_dx12.lock()->ProjectionMat();

	// �E�B���h�E�T�C�Y�͂Ƃ肠�����ׂ������ŁB
	auto screen = XMMatrixSet(
		960.0f / 2.0f,           0.0f, 0.0f, 0.0f,
		         0.0f, -540.0f / 2.0f, 0.0f, 0.0f,
		         0.0f,           0.0f, 1.0f, 0.0f,
		960.0f / 2.0f,  540.0f / 2.0f, 0.0f, 1.0f
	);

	auto resultVec = XMVectorSet(worldPos.x, worldPos.y, worldPos.z, 0.0f);
	resultVec = XMVector3Transform(resultVec, view);
	resultVec = XMVector3Transform(resultVec, projection);

	// w �Ŋ����Ă����Ȃ��Ⴂ���Ȃ����ۂ�
	resultVec = XMVectorSet(
		resultVec.m128_f32[0] / resultVec.m128_f32[3], // x / w
		resultVec.m128_f32[1] / resultVec.m128_f32[3], // y / w,
		resultVec.m128_f32[2] / resultVec.m128_f32[3], // z / w,
		resultVec.m128_f32[3] / resultVec.m128_f32[3]  // w / w,
	);

	resultVec = XMVector3Transform(resultVec, screen);

	auto result = XMFLOAT2{ resultVec.m128_f32[0], resultVec.m128_f32[1] };

	return result;
}

std::vector<XMFLOAT2> LineDrawer::ToScreenPosAll()
{
	std::vector<XMFLOAT2> result;

	for (auto& pos : m_worldPositions)
	{
		result.push_back(ToScreenPos(pos));
	}

	return result;
}

std::vector<XMFLOAT2> LineDrawer::SelectVertex(const std::vector<XMFLOAT2>& screenPositions)
{
	std::vector<XMFLOAT2> result;
	std::vector<XMFLOAT2> inScreenVertices;

	// ��ʓ��̂��̂�T��
	{
		for (auto& vertex : screenPositions)
		{
			if (vertex.x >= 0 && vertex.y >= 0 && vertex.x <= 960 && vertex.y <= 540)
			{
				inScreenVertices.push_back(vertex);
			}
		}
	}

	// ���łɑI�񂾒��_�������ۂɕK�v�ȃC���f�b�N�X��ێ����Ă���
	int eraseIndex = -1;

	// �ǉ����钸�_����������L�^���Ă������߂̕ϐ�
	XMFLOAT2 v{ 960.0f, 540.0f };

	// 1�ڂ̓_�i��ԏ�̓_�j��T��
	{
		for (int i = 0; i < inScreenVertices.size(); ++i)
		{
			if (inScreenVertices.at(i).y < v.y)
			{
				v = inScreenVertices.at(i);
				eraseIndex = i;
			}
		}

		// �ǉ����폜
		result.push_back(v);
		inScreenVertices.erase(inScreenVertices.begin() + eraseIndex);
	}

	float maxAngle = 0.0f;
	// 2�ڂ̓_��T��
	{
		// x ���ƕ��s�ȃx�N�g��
		XMVECTOR vec = XMVectorSet(
			0.0f - result[0].x, 0.0f, 0.0f, 0.0f);

		// �Ȃ��p����ԑ傫�����̂�T��
		for (int i = 0; i < inScreenVertices.size(); ++i)
		{
			XMVECTOR difference = XMVectorSubtract(XMLoadFloat2(&inScreenVertices[i]), XMLoadFloat2(&result[0]));

			float cos = XMVector2Dot(vec, difference).m128_f32[0] / (XMVector2Length(vec) * XMVector2Length(difference)).m128_f32[0];

			float angle = XMConvertToDegrees(std::acos(cos));

			if (angle > maxAngle)
			{
				maxAngle = angle;
				v = inScreenVertices[i];
				eraseIndex = i;
			}
		}

		// �ǉ����폜
		result.push_back(v);
		inScreenVertices.erase(inScreenVertices.begin() + eraseIndex);
	}

	// 3�ڈȍ~�̓_��T��
	{
		// 6�̒��_���I�΂ꂽ ���� �X�N���[����ɒ��_���܂�����ԃ��[�v
		while (result.size() < 6 && inScreenVertices.size() > 0)
		{
			maxAngle = 0.0f;
			eraseIndex = -1;
			int curIndex = result.size() - 1;

			for (int i = 0; i < inScreenVertices.size(); ++i)
			{
				// �ŐV�̓_��1�O�̓_�̃x�N�g��
				XMVECTOR vec1 = -XMVectorSubtract(
					XMLoadFloat2(&result.at(curIndex)),
					XMLoadFloat2(&result.at(curIndex - 1))
				);

				// �ŐV�̓_�Ǝ��̓_�̃x�N�g��
				XMVECTOR vec2 = XMVectorSubtract(
					XMLoadFloat2(&inScreenVertices.at(i)),
					XMLoadFloat2(&result.at(curIndex))
				);

				// �Ȃ��p�����߂�
				float cos = XMVector2Dot(vec1, vec2).m128_f32[0] / (XMVector2Length(vec1) * XMVector2Length(vec2)).m128_f32[0];
				float angle = XMConvertToDegrees(std::acos(cos));

				// �p�x����ԑ傫��
				if (angle > maxAngle)
				{
					// �ŐV�̓_��1�Ԗڂ̓_�̃x�N�g��
					XMVECTOR vec3 = XMVectorSubtract(
						XMLoadFloat2(&result.at(0)),
						XMLoadFloat2(&result.at(curIndex))
					);

					// �Ȃ��p�����߂�
					float cos2 = XMVector2Dot(vec1, vec3).m128_f32[0] / (XMVector2Length(vec1) * XMVector2Length(vec3)).m128_f32[0];
					float angle2 = XMConvertToDegrees(std::acos(cos2));

					// �ŏ��̓_�ƌ��񂾕����p�x���傫���Ȃ�
					if (angle2 > angle)
					{
						// �p�x��ێ�
						maxAngle = angle2;

						// ���������Ȃ�
						eraseIndex = -1;

						// ���̓_��
						continue;
					}

					// �p�x��ێ�
					maxAngle = angle;

					// ���_���W��ێ�
					v = inScreenVertices.at(i);

					// �폜�Ώۂ̃C���f�b�N�X��ێ�
					eraseIndex = i;
				}
			}

			// �폜�Ώۂ�����Ȃ����
			if (eraseIndex >= 0)
			{
				// �ǉ����폜
				result.push_back(v);
				inScreenVertices.erase(inScreenVertices.begin() + eraseIndex);
			}
			// �폜�Ώۂ��Ȃ� = �ŏ��̓_�ƌ��񂾕����傫�� = �}�`���������Ă���
			else
			{
				// while���[�v�I��
				break;
			}
		}
	}

	// �ŏ��̓_��ǉ����Đ}�`������
	result.push_back(result.at(0));

	return result;
}

void LineDrawer::UpdateVertices()
{
	m_vertices.clear();

	m_vertices = SelectVertex(ToScreenPosAll());
}
