#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <d3d12.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
using namespace DirectX;

//�u�����h���[�h���ʔԍ�
enum BlendMode
{
	noBlend,
	add,
	sub,
	invers,
	alpha,
};

class Pipeline
{
public:
	ID3DBlob* vsBlob = nullptr; // ���_�V�F�[�_�I�u�W�F�N�g
	ID3DBlob* psBlob = nullptr; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ID3DBlob* errorBlob = nullptr; // �G���[�I�u�W�F�N�g

	//���_���C�A�E�g
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;

	// �O���t�B�b�N�X�p�C�v���C��
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

public:
	void Initialize(ID3D12Device* device, int blendMode);
	void Update();

private:
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
	void SetBlendState(int blendMode);
	// ���_���C�A�E�g�̐ݒ�
	void SetVerLayout();
	// �}�`�̌`��ݒ�
	void SetTopology();
};

