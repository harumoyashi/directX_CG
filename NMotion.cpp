#include "NMotion.h"

void Motion::Initialize(ID3D12Device* device)
{
	for (int i = 0; i < kNumPartId; i++)
	{
		object3d[i].InitializeObject3d(device);
	}
	floor.InitializeObject3d(device);

	const float jointDistance = 15.0f;

	object3d[PartId::kRoot].position = { 0,0,0 };

	object3d[PartId::kSpine].position = { 0,jointDistance,0 };
	object3d[PartId::kSpine].parent = &object3d[PartId::kRoot];

	object3d[PartId::kChest].position = { 0,0,0 };
	object3d[PartId::kChest].parent = &object3d[PartId::kSpine];

	object3d[PartId::kHead].position = { 0,jointDistance,0 };
	object3d[PartId::kHead].scale = { 1.5f,1.5f,1.5f };
	object3d[PartId::kHead].parent = &object3d[PartId::kSpine];

	object3d[PartId::kUpArmL].position = { -jointDistance * 0.8f,-2.0f,0 };
	object3d[PartId::kUpArmL].rotation = { 0,0,-0.05f };
	object3d[PartId::kUpArmL].scale = { 0.5f,1.3f,0.5f };
	object3d[PartId::kUpArmL].parent = &object3d[PartId::kChest];

	object3d[PartId::kForeArmL].position = { 0,-jointDistance * 0.8f,0 };
	object3d[PartId::kForeArmL].scale = { 1.0f,1.0f,1.0f };
	object3d[PartId::kForeArmL].parent = &object3d[PartId::kUpArmL];

	object3d[PartId::kHandL].position = { 0,-jointDistance * 0.7f,0 };
	object3d[PartId::kHandL].scale = { 1.0f,0.5f,1.0f };
	object3d[PartId::kHandL].parent = &object3d[PartId::kForeArmL];

	object3d[PartId::kUpArmR].position = { jointDistance * 0.8f,-2.0f,0 };
	object3d[PartId::kUpArmR].rotation = { 0,0,0.05f };
	object3d[PartId::kUpArmR].scale = { 0.5f,1.3f,0.5f };
	object3d[PartId::kUpArmR].parent = &object3d[PartId::kChest];

	object3d[PartId::kForeArmR].position = { 0,-jointDistance * 0.8f,0 };
	object3d[PartId::kForeArmR].scale = { 1.0f,1.0f,1.0f };
	object3d[PartId::kForeArmR].parent = &object3d[PartId::kUpArmR];

	object3d[PartId::kHandR].position = { 0,-jointDistance * 0.7f,0 };
	object3d[PartId::kHandR].scale = { 1.0f,0.5f,1.0f };
	object3d[PartId::kHandR].parent = &object3d[PartId::kForeArmR];

	object3d[PartId::kWaist].position = { 0,-jointDistance,0 };
	object3d[PartId::kWaist].parent = &object3d[PartId::kSpine];

	object3d[PartId::kHip].position = { 0,-jointDistance,0 };
	object3d[PartId::kHip].parent = &object3d[PartId::kWaist];

	object3d[PartId::kUpLegRootL].position = { -jointDistance * 0.5f,0,0 };
	object3d[PartId::kUpLegRootL].parent = &object3d[PartId::kHip];

	object3d[PartId::kUpLegL].position = { 0,-jointDistance,0 };
	object3d[PartId::kUpLegL].scale = { 0.8f,2.0f,0.8f };
	object3d[PartId::kUpLegL].parent = &object3d[PartId::kUpLegRootL];

	object3d[PartId::kKneeL].position = { 0,-jointDistance * 0.3f,0 };
	object3d[PartId::kKneeL].scale = { 1.0f,0.5f,1.0f };
	object3d[PartId::kKneeL].parent = &object3d[PartId::kUpLegL];

	object3d[PartId::kLowLegL].position = { 0,-jointDistance * 0.8f,0 };
	object3d[PartId::kLowLegL].scale = { 1.0f,2.0f,1.0f };
	object3d[PartId::kLowLegL].parent = &object3d[PartId::kKneeL];

	object3d[PartId::kFootL].position = { 0,-jointDistance * 0.6f,-2.0f };
	object3d[PartId::kFootL].scale = { 1.0f,0.5f,1.3f };
	object3d[PartId::kFootL].parent = &object3d[PartId::kLowLegL];

	object3d[PartId::kUpLegRootR].position = { jointDistance * 0.5f,0,0 };
	object3d[PartId::kUpLegRootR].parent = &object3d[PartId::kHip];

	object3d[PartId::kUpLegR].position = { 0,-jointDistance,0 };
	object3d[PartId::kUpLegR].scale = { 0.8f,2.0f,0.8f };
	object3d[PartId::kUpLegR].parent = &object3d[PartId::kUpLegRootR];

	object3d[PartId::kKneeR].position = { 0,-jointDistance * 0.3f,0 };
	object3d[PartId::kKneeR].scale = { 1.0f,0.5f,1.0f };
	object3d[PartId::kKneeR].parent = &object3d[PartId::kUpLegR];

	object3d[PartId::kLowLegR].position = { 0,-jointDistance * 0.8f,0 };
	object3d[PartId::kLowLegR].scale = { 1.0f,2.0f,1.0f };
	object3d[PartId::kLowLegR].parent = &object3d[PartId::kKneeR];

	object3d[PartId::kFootR].position = { 0,-jointDistance * 0.6f,-2.0f };
	object3d[PartId::kFootR].scale = { 1.0f,0.5f,1.3f };
	object3d[PartId::kFootR].parent = &object3d[PartId::kLowLegR];

	/*object3d[PartId::kFloor].position = { 0,-920.0f,0 };
	object3d[PartId::kFloor].scale = { 30.0f,1.0f,30.0f };
	object3d[PartId::kFloor].parent = &object3d[PartId::kRoot];*/

	floor.position = { 0,-90.0f,0 };
	floor.scale = { 30.0f,1.0f,30.0f };
}

void Motion::Update(XMMATRIX matView, XMMATRIX matProjection)
{
	speed = PI * speedAmount;
	rotSpeed = timer;	//��{�̉�]�̓^�C�}�[�Ɠ����l
	easeInRotSpeed = maxHalfTimer * EaseIn(halfTimer / maxHalfTimer, easeSpeed) + PI / 2 * swingVec;	//�r�̕t�����̓C�[�W���O�����āA�������炵�Ă���

	//��������߂�����
	easeInRotSpeed = clamp(easeInRotSpeed, -PI - PI / 2, PI + PI / 2);

	if (timer == maxTimer)
	{
		timer = 0;	//�^�C�}�[���Z�b�g
	}

	if (halfTimer == maxHalfTimer)
	{
		swingVec = -swingVec;	//�U��������t������
		halfTimer = 0;			//�^�C�}�[���Z�b�g
	}

#pragma region ��]����
	/*if (key.IsKeyDown(DIK_RETURN))
	{*/
	if (isMoveMode)
	{
		RunMode();
	}
	else
	{
		WalkMode();
	}
	/*}*/
#pragma endregion
	for (size_t i = 0; i < kNumPartId; i++)
	{
		object3d[i].UpdateObject3d(matView, matProjection);
	}
	floor.UpdateObject3d(matView, matProjection);	//��
}

void Motion::Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW vbView, D3D12_INDEX_BUFFER_VIEW ibView, int indicesSize)
{
	for (int i = kChest; i < kNumPartId; i++)
	{
		object3d[i].DrawObject3d(commandList, vbView, ibView, indicesSize);
	}
	floor.DrawObject3d(commandList, vbView, ibView, indicesSize);	//��
}

void Motion::StartTimer()
{
	/*if (key.IsKeyDown(DIK_RETURN))
	{*/
	timer += speed;
	if (timer > maxTimer)
	{
		timer = clamp(timer, 0.0f, maxTimer);
	}

	//�n�[�t�^�C�}�[
	halfTimer += speed;
	if (halfTimer > maxHalfTimer)
	{
		halfTimer = clamp(halfTimer, 0.0f, maxHalfTimer);
	}


	/*}*/
}

void Motion::RotationKey()
{
	////���W����
	//if (key.IsKeyDown(DIK_UP) || key.IsKeyDown(DIK_DOWN) || key.IsKeyDown(DIK_RIGHT) || key.IsKeyDown(DIK_LEFT))
	//{
	//	if (key.IsKeyDown(DIK_UP)) { object3d[0].position.y += 1.0f; }
	//	else if (key.IsKeyDown(DIK_DOWN)) { object3d[0].position.y -= 1.0f; }
	//	if (key.IsKeyDown(DIK_RIGHT)) { object3d[0].position.x += 1.0f; }
	//	else if (key.IsKeyDown(DIK_LEFT)) { object3d[0].position.x -= 1.0f; }
	//}

	//�㔼�g��]
	if (key.IsKeyDown(DIK_U) || key.IsKeyDown(DIK_I))
	{
		if (key.IsKeyDown(DIK_U)) { object3d[kChest].rotation.y += 0.05f; }
		else if (key.IsKeyDown(DIK_I)) { object3d[kChest].rotation.y -= 0.05f; }
	}

	//�����g��]
	if (key.IsKeyDown(DIK_J) || key.IsKeyDown(DIK_K))
	{
		if (key.IsKeyDown(DIK_J)) { object3d[kHip].rotation.y += 0.05f; }
		else if (key.IsKeyDown(DIK_K)) { object3d[kHip].rotation.y -= 0.05f; }
	}

	//�X�s�[�h�ύX
	if (key.IsKeyTrigger(DIK_UP) || key.IsKeyTrigger(DIK_DOWN))
	{
		if (speedAmount < 0.2f && speedAmount >= 0.02f)
		{
			if (key.IsKeyTrigger(DIK_UP)) speedAmount += 0.01f;
			else if (key.IsKeyTrigger(DIK_DOWN)) speedAmount -= 0.01f;
		}
	}

	if (key.IsKeyTrigger(DIK_R))
	{
		if (!isMoveMode)
		{
			isMoveMode = true;
			speedAmount = 0.05f;
			easeSpeed = 4;
		}
		else
		{
			isMoveMode = false;
			speedAmount = 0.02f;
			easeSpeed = 2;
		}
	};
}

void Motion::RunMode()
{
	//��̘r�̐U��
	object3d[kUpArmL].rotation.x = 1.2f * sinf(-easeInRotSpeed);
	object3d[kUpArmR].rotation.x = 1.2f * sinf(easeInRotSpeed);
	//�O�r�̐U��
	object3d[kForeArmL].rotation.x = 0.3f * sinf(rotSpeed) + 0.9f;
	object3d[kForeArmR].rotation.x = 0.3f * sinf(-rotSpeed) + 0.9f;
	//�r�t�����̉�]
	object3d[kUpLegRootL].rotation.x = sinf(easeInRotSpeed) + 0.2f;
	object3d[kUpLegRootR].rotation.x = sinf(-easeInRotSpeed) + 0.2f;
	//�G�̉�](�G�̕`��Ȃ�)
	object3d[kKneeL].rotation.x = 0.8f * sinf(-easeInRotSpeed) - 1.2f;
	object3d[kKneeR].rotation.x = 0.8f * sinf(easeInRotSpeed) - 1.2f;
	//���̉�]
	object3d[kFootL].rotation.x = 0.2f * sinf(easeInRotSpeed) + 0.2f;
	object3d[kFootR].rotation.x = 0.2f * sinf(-easeInRotSpeed) + 0.2f;
	//�d�S�ړ�
	object3d[kSpine].position.y = 4.0f * sinf(rotSpeed * 2.0f) + 2.0f;
	object3d[kSpine].rotation.x = 0.05f * sinf(easeInRotSpeed * 2.0f) - 0.1f;
	//���̔P��
	object3d[kChest].rotation.y = 0.2f * sinf(easeInRotSpeed);
	//������̔P��
	object3d[kHip].rotation.y = 0.1f * -sinf(easeInRotSpeed);
}

void Motion::WalkMode()
{
	//��̘r�̐U��
	object3d[kUpArmL].rotation.x = 0.5f * sinf(-easeInRotSpeed);
	object3d[kUpArmR].rotation.x = 0.5f * sinf(easeInRotSpeed);
	//�O�r�̐U��
	object3d[kForeArmL].rotation.x = 0.2f * (sinf(rotSpeed) + 0.9f);
	object3d[kForeArmR].rotation.x = 0.2f * (sinf(-rotSpeed) + 0.9f);
	//�r�t�����̉�]
	object3d[kUpLegRootL].rotation.x = 0.4f * sinf(easeInRotSpeed) + 0.2f;
	object3d[kUpLegRootR].rotation.x = 0.4f * sinf(-easeInRotSpeed) + 0.2f;
	//�G�̉�](�G�̕`��Ȃ�)
	object3d[kKneeL].rotation.x = 0.5f * sinf(easeInRotSpeed) - 0.6f;
	object3d[kKneeR].rotation.x = 0.5f * sinf(-easeInRotSpeed) - 0.6f;
	//���̉�]						     
	object3d[kFootL].rotation.x = 0.1f * sinf(easeInRotSpeed) + 0.1f;
	object3d[kFootR].rotation.x = 0.1f * sinf(-easeInRotSpeed) + 0.1f;
	//�d�S�ړ�						     
	object3d[kSpine].position.y = 0.7f * sinf(rotSpeed * 2.0f) + 2.0f;
	object3d[kSpine].rotation.x = 0.02f * sinf(easeInRotSpeed * 2.0f) - 0.03f;
	//���̔P��						     
	object3d[kChest].rotation.y = 0.1f * sinf(easeInRotSpeed);
	//������̔P��				   	       
	object3d[kHip].rotation.y = 0.1f * -sinf(easeInRotSpeed);
}

float Motion::clamp(float value, float min, float max)
{
	if (value < min)
	{
		return min;
	}
	else if (value > max)
	{
		return max;
	}
	return value;
}
