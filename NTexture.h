#pragma once
#include "NDirectX.h"

class Texture
{
private:
	HRESULT result;

public:
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	ScratchImage mipChain{};					//�~�b�v�}�b�v
	D3D12_RESOURCE_DESC textureResouceDesc{};	//���\�[�X�\����
	ID3D12Resource* texBuff = nullptr;			//�萔�o�b�t�@
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//�ݒ�\����

	UINT incrementSize;	//���̃e�N�X�`�����̏ꏊ�ɐi�ނ��߂Ɏg��

public:
	//WIC�e�N�X�`���̃��[�h
	void Load(const wchar_t* pictureName);
	//�~�b�v�}�b�v����
	void CreateMipmap();
	//���\�[�X�ݒ�
	void SetResouce();
	//�e�N�X�`���o�b�t�@�̐���
	void CreateTexBuff(ID3D12Device* device, D3D12_HEAP_PROPERTIES textureHeapProp);
	//�~�b�v�}�b�v�f�[�^�̓]��
	void MipmapDataSend();
	//�V�F�[�_�[���\�[�X�r���[�쐬
	void CreateSRV(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE srvHandle);
	//1�n���h����i�߂�
	void NextHandle(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE srvHandle);
};

const int maxTexture = 3;	//�g�p����e�N�X�`���̖���