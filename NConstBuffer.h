#pragma once
#include "NDirectX.h"

#pragma region �\���̐錾
//�萔�o�b�t�@�p�\����
struct ConstBufferDataMaterial
{
	XMFLOAT4 color;	//�F(RGBA)
};

class ConstBuff
{
public:
	ID3D12Resource* constBuffMaterial = nullptr;
	D3D12_HEAP_PROPERTIES cbHeapProp{};		//�q�[�v�ݒ�
	D3D12_RESOURCE_DESC cbResouceDesc{};	//���\�[�X�ݒ�
	ConstBufferDataMaterial* constMapMaterial = nullptr;	//�萔�o�b�t�@�̃}�b�v

private:


public:
	//�q�[�v�ݒ�
	void SetHeapProp();
	//���\�[�X�ݒ�
	void SetResoucedesc();
	//�萔�o�b�t�@�̐���
	void Create(ID3D12Device* device);
	//�萔�o�b�t�@�̃}�b�s���O
	void Mapping();

	//�l���������ނƎ����I�ɓ]�������
	void TransfarColor(float R, float G, float B, float A);
};