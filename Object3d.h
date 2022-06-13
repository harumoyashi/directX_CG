#pragma once
#include "MyDirectX.h"

//�e�q�\���ԍ����蓖��
enum PartId
{
	kRoot,		//�匳
	kSpine,		//�Ғ�
	kUpLegRootL,//���r�t����
	kUpLegRootR,//�E�r�t����
	kKneeL,		//���Ђ�
	kKneeR,		//�E�Ђ�
	kChest,		//��
	kHead,		//��
	kUpArmL,	//����̘r
	kForeArmL,	//���O�r
	kHandL,		//����
	kUpArmR,	//�E��̘r
	kForeArmR,	//�E�O�r
	kHandR,		//�E��
	kWaist,		//��
	kHip,		//������
	kUpLegL,	//�����
	kLowLegL,	//������
	kFootL,		//����
	kUpLegR,	//�E���
	kLowLegR,	//�E����
	kFootR,		//�E��

	kFloor,		//��

	kNumPartId	//�\���̂̐�
};

class Object3d
{
public:
	//�萔�o�b�t�@�i�s��p�j
	ID3D12Resource* constBuffTransform;
	//�萔�o�b�t�@�}�b�v�i�s��p�j
	MyDirectX::ConstBufferDataTransform* constMapTransform;
	//�A�t�B���ϊ����
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 position = { 0,0,0 };
	//���[���h�ϊ��s��
	XMMATRIX matWorld;
	//�e�I�u�W�F�N�g�ւ̃|�C���^
	Object3d* parent = nullptr;

public:
	//������
	void Initialize(ID3D12Device* device);
	//�X�V
	void Update(XMMATRIX& matView, XMMATRIX& matProjection);
	//�`��
	void Draw(ID3D12GraphicsCommandList* commandList,
		D3D12_VERTEX_BUFFER_VIEW& vbView, D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices);

	void SetRot();
};

