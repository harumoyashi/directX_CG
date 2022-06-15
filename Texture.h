#pragma once
#include "MyDirectX.h"
#include <DirectXMath.h>

class Texture
{
public:
	HRESULT result;

	TexMetadata metadate{};
	ScratchImage scratchImg{};
	ScratchImage mipChain{};

	D3D12_HEAP_PROPERTIES textureHeapProp{};
	D3D12_RESOURCE_DESC textureResouceDesc{};
	ID3D12Resource* texBuff = nullptr;

	//���_�f�[�^�\����
	struct Vertex
	{
		XMFLOAT3 pos;	//xyz���W
		XMFLOAT2 uv;	//uv���W
	};

	std::vector<Vertex> vertices;

public:
	//������
	void Initialize();

	//WIC�e�N�X�`���̃��[�h
	void Lord(const wchar_t* name_format);
	//�~�b�v�}�b�v����
	void CreateMipMap();
	//�ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	void SetSRGB();

	void SetTexBuff();
	void CreateTexBuff(ID3D12Device* device);
	//�S�~�b�v�}�b�v�ɂ���
	void SetMipMap();
};

