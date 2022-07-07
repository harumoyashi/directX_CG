#pragma once
#include "NDirectXInput.h"
#include "NObject.h"
#include "NEasing.h"

class Motion
{
private:
	//���݂̃X�s�[�h�A�ő�l�A�ŏ��l�A�����x
	struct Speed
	{
		float spd;
		const float min;
		const float max;
		const float spdAmount = (max - min) / 10.0f;
	};

	Speed speedAmount = {0.02f,0.02f,0.1f};
	float speed = PI * speedAmount.spd;	//�ړ����x�W��&�^�C�}�[���x
	float timer = 0;				//���[�V�����^�C�}�[
	float halfTimer = 0;			//���[�V�����^�C�}�[(�n�[�t)
	const float maxTimer = PI * 2;
	const float maxHalfTimer = PI;

	float rotSpeed;			//�U�鑬�x
	float easeInRotSpeed;	//�C�[�W���O�|�����o�[�W����
	Speed easeSpeed = { 2.0f,2.0f,4.0f };	//�C�[�W���O�̉���
	float swingVec = 1.0f;	//�r�⑫��U�����

	float angle = 0.0f;	//���邮�鑖�点��p�̊p�x

	//�e���ʂ̉�]�X�s�[�h�ݒ�
	Speed upArmSpd =      { 0.5f,0.5f,1.2f };
	Speed foreArmSpd =    { 0.2f,0.2f,0.3f };
	Speed upLegSpd =      { 0.4f,0.4f,1.0f };
	Speed kneeSpd =       { 0.5f,0.5f,0.8f };
	Speed footSpd =       { 0.1f,0.1f,0.2f };
	Speed centroidSpd =   { 0.7f,0.7f,4.0f };
	Speed bodyTiltSpd =   { 0.02f,0.02f,0.05f };
	Speed chestTwistSpd = { 0.1f,0.1f,0.2f };
	Speed elbowPlusSpd =  { 0.1f,0.1f,1.2f };
	Speed kneePlusSpd =   { 0.6f,0.6f,1.2f };

	float rotVec = -1.0f;	//true:run,false:walk

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

