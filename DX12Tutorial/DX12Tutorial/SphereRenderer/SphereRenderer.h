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

// ���̂̕`����s���N���X
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

	// ���_�o�b�t�@�[
	ComPtr<ID3D12Resource> m_vertexBuffer{ nullptr };
	// ���_�o�b�t�@�[�r���[
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};
	// �C���f�b�N�X�o�b�t�@�[
	ComPtr<ID3D12Resource> m_indexBuffer{ nullptr };
	// �C���f�b�N�X�o�b�t�@�[�r���[
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView{};
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> m_constantBuffer{ nullptr };
	// �萔�o�b�t�@�[�p�f�B�X�N���v�^�[�q�[�v
	ComPtr<ID3D12DescriptorHeap> m_cbvHeap{ nullptr };
	// ���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> m_rootSignature{ nullptr };
	// ���_�V�F�[�_�[
	ComPtr<ID3DBlob> m_vertexShader{ nullptr };
	// �s�N�Z���V�F�[�_�[
	ComPtr<ID3DBlob> m_pixelShader{ nullptr };
	// �p�C�v���C��
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

