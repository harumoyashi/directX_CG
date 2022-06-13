#include "Motion.h"

void Motion::StartButton()
{
	/*if (key.IsKeyDown(DIK_SPACE))
	{*/
		rotSpeed += 0.05f;

		if (sinf(rotSpeed) <= 0.0f)
		{
			lKneeRotSpeed += 0.1f;
		}
		else
		{
			rKneeRotSpeed += 0.1f;
		}
	//}
	/*else
	{
		rotSpeed = PI;
	}*/
}

void Motion::SetRotPara()
{
	//��]����ϐ��Ɋi�[//
	//��̘r�̐U��
	rot[kUpArmL].x = 0.5f * sinf(-rotSpeed);
	rot[kUpArmR].x = 0.5f * sinf(rotSpeed);
	//�O�r�̐U��
	rot[kForeArmL].x = 0.2f * (cosf(rotSpeed) + 0.9f);
	rot[kForeArmR].x = 0.2f * (sinf(rotSpeed) + 0.9f);

	//�r�t�����̉�]
	rot[kUpLegRootL].x = /*EaseInSine(*/0.3f * sinf(rotSpeed) + 0.2f/*)*/;
	rot[kUpLegRootR].x = 0.3f * sinf(-rotSpeed) + 0.2f;
	////�G�̉�](�G�̕`��Ȃ�)
	//rot[kKneeL].x = 0.5f * sinf(lKneeRotSpeed + 1.0f) - 0.5f;
	//rot[kKneeR].x = 0.5f * sinf(rKneeRotSpeed + 1.0f) - 0.5f;
	////���̉�]
	//rot[kFootL].x = 0.1f * sinf(rotSpeed) + 0.2f;
	//rot[kFootR].x = 0.1f * sinf(-rotSpeed) + 0.2f;

	//�d�S�ړ�
	spineTrans.y = 0.5f * -sinf(rotSpeed * 2.0f) + 2.0f;
	rot[kSpine].x = 0.02f * -sinf(rotSpeed * 2.0f) - 0.03f;

	//���̔P��
	rot[kChest].y = 0.1f * sinf(rotSpeed);
	//������̔P��
	rot[kHip].y = 0.1f * -sinf(rotSpeed);
}

void Motion::SetRot()
{
	//�i�[���Ă�������]������//
	//��̘r�̐U��
	object3ds[kUpArmL].rotation.x = rot[kUpArmL].x;
	object3ds[kUpArmR].rotation.x = rot[kUpArmR].x;
	//�O�r�̐U��
	object3ds[kForeArmL].rotation.x = rot[kForeArmL].x;
	object3ds[kForeArmR].rotation.x = rot[kForeArmR].x;

	//�r�t�����̉�]
	object3ds[kUpLegRootL].rotation.x = rot[kUpLegRootL].x;
	object3ds[kUpLegRootR].rotation.x = rot[kUpLegRootR].x;
	//�G�̉�](�G�̕`��Ȃ�)
	object3ds[kKneeL].rotation.x = rot[kKneeL].x;
	object3ds[kKneeR].rotation.x = rot[kKneeR].x;
	////���̉�]
	//object3ds[kFootL].rotation.x = objectRot[kFootL].x;
	//object3ds[kFootR].rotation.x = objectRot[kFootR].x;

	//�d�S�ړ�
	object3ds[kSpine].position.y = spineTrans.y;
	object3ds[kSpine].rotation.x = rot[kSpine].x;

	//���̔P��
	object3ds[kChest].rotation.y = rot[kChest].y;
	//������̔P��
	object3ds[kHip].rotation.y = rot[kHip].y;
}
