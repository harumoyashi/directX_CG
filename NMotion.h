#pragma once
#include "NDirectXInput.h"
#include "NObject.h"

class Motion
{
private:
	int timer = 0;	//���[�V�����^�C�}�[
	const int maxTimer = 300;

	DirectXInput key;	//�L�[�{�[�h

public:
	//3D�I�u�W�F�N�g�̔z��
	Object3d object3d[kNumPartId];

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

