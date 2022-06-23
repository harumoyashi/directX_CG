#pragma once
#include "NDirectXInput.h"
#include "NObject.h"
#include "NEasing.h"

class Motion
{
private:
	float speed = 0.05f;	//�ړ����x�W��&�^�C�}�[���x
	float timer = 0;		//���[�V�����^�C�}�[
	float halfTimer = 0;	//���[�V�����^�C�}�[(�n�[�t)
	const float maxTimer = PI*2;
	const float maxHalfTimer = PI;

	float rotSpeed;
	float legRotSpeed;

	DirectXInput key;	//�L�[�{�[�h

public:
	//3D�I�u�W�F�N�g�̔z��
	Object3d object3d[kNumPartId];

	Object3d floor;

public:
	//������
	void Initialize(ID3D12Device* device);
	//�X�V
	void Update(XMMATRIX matView, XMMATRIX matProjection);
	//�`��
	void Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW vbView, D3D12_INDEX_BUFFER_VIEW ibView, int indicesSize);
	//�G���^�[�����Ă�ԃ^�C�}�[�i��
	void StartTimer();
	//�L�[���͉�]����
	void RotationKey();
};

