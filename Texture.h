#pragma once
#include "MyDirectX.h"

class Texture
{
private:
	HRESULT result;

public:
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	ScratchImage mipChain{};
	D3D12_RESOURCE_DESC textureResouceDesc{};
	ID3D12Resource* texBuff = nullptr;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//�ݒ�\����

	UINT incrementSize;

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
	void CreateSRV(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE srvHandle);
	//1�n���h����i�߂�
	void NextHandle(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE srvHandle);
};

const int maxTexture = 2;