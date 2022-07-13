#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <d3d12.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
using namespace DirectX;

class Pipeline
{
public:
	// ���_�f�[�^
	std::vector<XMFLOAT3>vertices;
	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB;

	D3D12_HEAP_PROPERTIES heapProp{}; // �q�[�v�ݒ�
	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	// ���_�o�b�t�@
	ID3D12Resource* vertBuff = nullptr;
	//���_�f�[�^�R�s�[�p
	XMFLOAT3* vertMap = nullptr;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	ID3DBlob* vsBlob = nullptr; // ���_�V�F�[�_�I�u�W�F�N�g
	ID3DBlob* psBlob = nullptr; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ID3DBlob* errorBlob = nullptr; // �G���[�I�u�W�F�N�g

	//���_���C�A�E�g
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;

	// �O���t�B�b�N�X�p�C�v���C���X�e�[�g
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

public:
	void Initialize(ID3D12Device* device);
	void Update();

private:
	//���_�f�[�^�ݒ�
	void SetVertices();
	//���_�o�b�t�@�ݒ�
	void SetVerBuff();
	// ���_�o�b�t�@�̐���
	void CreateVerBuff(ID3D12Device* device);
	//���_�o�b�t�@�ւ̃f�[�^�]��
	void TransferVerBuff();
	// ���_�o�b�t�@�r���[�̍쐬
	void CreateVerBuffView();

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	void VSCompile();
	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	void PSCompile();
	// ���_���C�A�E�g
	void CreateVerLayout();

	// �V�F�[�_�[�̐ݒ�
	void SetShader();
	// ���X�^���C�U�̐ݒ�
	void SetRasterizer();
	// �u�����h�X�e�[�g�ݒ�
	void SetBlendState();
	// ���_���C�A�E�g�̐ݒ�
	void SetVerLayout();
	// �}�`�̌`��ݒ�
	void SetTopology();
};

