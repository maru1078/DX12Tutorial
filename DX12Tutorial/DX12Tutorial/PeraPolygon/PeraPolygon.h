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

	bool CreateVertexBuffer();
	bool CreateVertexBufferView();
	bool CreateRootSignature();
	bool CreateVertexShader();
	bool CreatePixelShader();
	bool CreatePipelineState();


private:

	std::weak_ptr<Dx12> m_dx12;

	// ���_�o�b�t�@�[
	ComPtr<ID3D12Resource> m_vertexBuffer{ nullptr };
	// ���_�o�b�t�@�[�r���[
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};
	// ���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> m_rootSignature{ nullptr };
	// ���_�V�F�[�_�[
	ComPtr<ID3DBlob> m_vertexShader{ nullptr };
	// �C���f�b�N�X�V�F�[�_�[
	ComPtr<ID3DBlob> m_pixelShader{ nullptr };
	// �p�C�v���C��
	ComPtr<ID3D12PipelineState> m_pipeline{ nullptr };
};

#endif // !PERA_POLYGON_H_

