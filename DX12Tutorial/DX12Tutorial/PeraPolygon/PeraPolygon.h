#ifndef PERA_POLYGON_H_
#define PERA_POLYGON_H_

#include <memory>
#include <d3d12.h>
#include <wrl.h>
#include <DirectXMath.h>
using namespace Microsoft::WRL;
using namespace DirectX;

class Dx12;

struct PeraVertex
{
	XMFLOAT2 position;
	XMFLOAT2 uv;
};

class PeraPolygon
{
public:

	PeraPolygon(std::weak_ptr<Dx12> dx12);

	void PreDrawPera();
	void DrawPera();

private:

	std::weak_ptr<Dx12> m_dx12;

	// 頂点バッファー
	ComPtr<ID3D12Resource> m_vertexBuffer{ nullptr };
	// 頂点バッファービュー
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};
	// ルートシグネチャ
	ComPtr<ID3D12RootSignature> m_rootSignature{ nullptr };
	// パイプライン
	ComPtr<ID3D12PipelineState> m_pipeline{ nullptr };
};

#endif // !PERA_POLYGON_H_

