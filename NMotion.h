#pragma once
#include "NDirectXInput.h"
#include "NObject.h"
#include "NEasing.h"

class Motion
{
private:
	float speedAmount = 0.05f;
	float speed = PI * speedAmount;	//�ړ����x�W��&�^�C�}�[���x
	float timer = 0;				//���[�V�����^�C�}�[
	float halfTimer = 0;			//���[�V�����^�C�}�[(�n�[�t)
	const float maxTimer = PI * 2;
	const float maxHalfTimer = PI;

	float rotSpeed;			//�U�鑬�x
	float easeInRotSpeed;	//�C�[�W���O�|�����o�[�W����
	int easeSpeed = 4;		//�C�[�W���O�̉���
	float swingVec = 1.0f;	//�r�⑫��U�����

	bool isMoveMode = true;	//true:run,false:walk

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
	//���胂�[�h
	void RunMode();
	//�������[�h
	void WalkMode();

	float clamp(float value,float min,float max);
};

