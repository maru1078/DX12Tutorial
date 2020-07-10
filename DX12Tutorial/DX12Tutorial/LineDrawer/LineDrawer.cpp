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
	// 頂点座標を更新（どの頂点を使うかが変わってくる）
	UpdateVertices();

	// 「頂点2つを選んで描画」を繰り返す
	for (int i = 0; i < m_vertices.size() - 1; ++i)
	{
		// 頂点バッファーの更新
		XMFLOAT2* vertMap{ nullptr };
		m_vertexBuffer->Map(0, nullptr, (void**)&vertMap);
		std::copy(m_vertices.begin() + i, m_vertices.begin() + i + 2, vertMap);
		m_vertexBuffer->Unmap(0, nullptr);

		// レンダーターゲットセット
		m_dx12.lock()->SetRenderTarget();

		// ビューポートセット
		m_dx12.lock()->SetViewPort();

		// シザー矩形セット
		m_dx12.lock()->SetScissorRect();

		// ルートシグネチャセット
		m_dx12.lock()->CommandList()->SetGraphicsRootSignature(m_rootSignature.Get());

		// パイプラインセット
		m_dx12.lock()->CommandList()->SetPipelineState(m_pipeline.Get());

		// 頂点バッファービューセット
		m_dx12.lock()->CommandList()->IASetVertexBuffers(0, 1, &m_vertexBufferView);

		// ヒープセット
		m_dx12.lock()->CommandList()->SetDescriptorHeaps(1, m_cbvHeap.GetAddressOf());

		// ヒープとルートパラメーターの関連付け
		m_dx12.lock()->CommandList()->SetGraphicsRootDescriptorTable(
			0,
			m_cbvHeap->GetGPUDescriptorHandleForHeapStart());

		// プリミティブトポロジーセット
		m_dx12.lock()->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		// ドローコール
		m_dx12.lock()->CommandList()->DrawInstanced(2, 1, 0, 0);

		// コマンドリスト実行
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
	resDesc.Width = sizeof(XMFLOAT2) * 2; // 頂点2つ分
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;

	// バッファー作成
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

	// バッファー作成
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
	// インプットレイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
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

	// ウィンドウサイズはとりあえずべた書きで。
	auto screen = XMMatrixSet(
		960.0f / 2.0f,           0.0f, 0.0f, 0.0f,
		         0.0f, -540.0f / 2.0f, 0.0f, 0.0f,
		         0.0f,           0.0f, 1.0f, 0.0f,
		960.0f / 2.0f,  540.0f / 2.0f, 0.0f, 1.0f
	);

	auto resultVec = XMVectorSet(worldPos.x, worldPos.y, worldPos.z, 0.0f);
	resultVec = XMVector3Transform(resultVec, view);
	resultVec = XMVector3Transform(resultVec, projection);

	// w で割ってあげなきゃいけないっぽい
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

	// 画面内のものを探す
	{
		for (auto& vertex : screenPositions)
		{
			if (vertex.x >= 0 && vertex.y >= 0 && vertex.x <= 960 && vertex.y <= 540)
			{
				inScreenVertices.push_back(vertex);
			}
		}
	}

	// すでに選んだ頂点を消す際に必要なインデックスを保持しておく
	int eraseIndex = -1;

	// 追加する頂点をいったん記録しておくための変数
	XMFLOAT2 v{ 960.0f, 540.0f };

	// 1つ目の点（一番上の点）を探す
	{
		for (int i = 0; i < inScreenVertices.size(); ++i)
		{
			if (inScreenVertices.at(i).y < v.y)
			{
				v = inScreenVertices.at(i);
				eraseIndex = i;
			}
		}

		// 追加＆削除
		result.push_back(v);
		inScreenVertices.erase(inScreenVertices.begin() + eraseIndex);
	}

	float maxAngle = 0.0f;
	// 2つ目の点を探す
	{
		// x 軸と平行なベクトル
		XMVECTOR vec = XMVectorSet(
			0.0f - result[0].x, 0.0f, 0.0f, 0.0f);

		// なす角が一番大きいものを探す
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

		// 追加＆削除
		result.push_back(v);
		inScreenVertices.erase(inScreenVertices.begin() + eraseIndex);
	}

	// 3つ目以降の点を探す
	{
		// 6つの頂点が選ばれた かつ スクリーン上に頂点がまだある間ループ
		while (result.size() < 6 && inScreenVertices.size() > 0)
		{
			maxAngle = 0.0f;
			eraseIndex = -1;
			int curIndex = result.size() - 1;

			for (int i = 0; i < inScreenVertices.size(); ++i)
			{
				// 最新の点と1つ前の点のベクトル
				XMVECTOR vec1 = -XMVectorSubtract(
					XMLoadFloat2(&result.at(curIndex)),
					XMLoadFloat2(&result.at(curIndex - 1))
				);

				// 最新の点と次の点のベクトル
				XMVECTOR vec2 = XMVectorSubtract(
					XMLoadFloat2(&inScreenVertices.at(i)),
					XMLoadFloat2(&result.at(curIndex))
				);

				// なす角を求める
				float cos = XMVector2Dot(vec1, vec2).m128_f32[0] / (XMVector2Length(vec1) * XMVector2Length(vec2)).m128_f32[0];
				float angle = XMConvertToDegrees(std::acos(cos));

				// 角度が一番大きい
				if (angle > maxAngle)
				{
					// 最新の点と1番目の点のベクトル
					XMVECTOR vec3 = XMVectorSubtract(
						XMLoadFloat2(&result.at(0)),
						XMLoadFloat2(&result.at(curIndex))
					);

					// なす角を求める
					float cos2 = XMVector2Dot(vec1, vec3).m128_f32[0] / (XMVector2Length(vec1) * XMVector2Length(vec3)).m128_f32[0];
					float angle2 = XMConvertToDegrees(std::acos(cos2));

					// 最初の点と結んだ方が角度が大きいなら
					if (angle2 > angle)
					{
						// 角度を保持
						maxAngle = angle2;

						// 何も消さない
						eraseIndex = -1;

						// 次の点へ
						continue;
					}

					// 角度を保持
					maxAngle = angle;

					// 頂点座標を保持
					v = inScreenVertices.at(i);

					// 削除対象のインデックスを保持
					eraseIndex = i;
				}
			}

			// 削除対象があるなら消す
			if (eraseIndex >= 0)
			{
				// 追加＆削除
				result.push_back(v);
				inScreenVertices.erase(inScreenVertices.begin() + eraseIndex);
			}
			// 削除対象がない = 最初の点と結んだ方が大きい = 図形が完成している
			else
			{
				// whileループ終了
				break;
			}
		}
	}

	// 最初の点を追加して図形を完成
	result.push_back(result.at(0));

	return result;
}

void LineDrawer::UpdateVertices()
{
	m_vertices.clear();

	m_vertices = SelectVertex(ToScreenPosAll());
}
