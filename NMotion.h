#pragma once
#include "NDirectXInput.h"
#include "NInputPad.h"
#include "NObject.h"
#include "NEasing.h"

class Motion
{
	struct Speed
	{
		float spd;
		const float min;
		const float max;
		float spdAmount = (max - min) * 0.1f;
	};

private:
	Speed speedAmount = { 0.02f,0.02f,0.1f };
	float speed = PI * speedAmount.spd;	//�ړ����x�W��&�^�C�}�[���x
	float timer = 0;				//���[�V�����^�C�}�[
	float halfTimer = 0;			//���[�V�����^�C�}�[(�n�[�t)
	const float maxTimer = PI * 2;
	const float maxHalfTimer = PI;

	float rotSpeed;			//�U�鑬�x
	float easeInRotSpeed;	//�C�[�W���O�|�����o�[�W����
	Speed easeSpeed = { 2.0f,2.0f,4.0f };	//�C�[�W���O�̉���
	float swingVec = 1.0f;	//�r�⑫��U�����

	float angle = 0.0f;

	const float spdNorm = 4.0f;

	//�e���ʂ̉�]�X�s�[�h�ݒ�
	Speed upArmSpd = { 0.5f,0.5f,1.2f };
	Speed foreArmSpd = { 0.2f,0.2f,0.3f };
	Speed upLegSpd = { 0.4f,0.4f,1.0f };
	Speed kneeSpd = { 0.5f,0.5f,0.8f };
	Speed footSpd = { 0.1f,0.1f,0.2f };
	Speed centroidSpd = { 0.7f,0.7f,4.0f };
	Speed bodyTiltSpd = { 0.02f,0.02f,0.05f };
	Speed chestTwistSpd = { 0.1f,0.1f,0.2f };
	Speed elbowPlusSpd = { 0.1f,0.1f,1.2f };
	Speed kneePlusSpd = { 0.6f,0.6f,1.2f };

	float rotVec = -1.0f;	//true:run,false:walk

	DirectXInput key;	//�L�[�{�[�h
	InputPad pad;

	Vector2 stickVec = {};
	float len;
	Vector2 frontVec = { 0,1 };

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
	//���W����
	void Operation();
	//��]����
	void RotUpdate();

	//臒l�����Ȃ��悤��
	float Clamp(float value, float min, float max);
	//�X�s�[�h��pClamp
	float SpeedClamp(Speed Spd);
};

