#include "NMotion.h"

void Motion::Initialize(ID3D12Device* device)
{
	object3d.InitializeObject3d(device);
}

void Motion::Update(XMMATRIX matView, XMMATRIX matProjection)
{
}

void Motion::Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW vbView, D3D12_INDEX_BUFFER_VIEW ibView, int indicesSize)
{
	object3d.DrawObject3d(commandList, vbView, ibView, indicesSize);
	floor.DrawObject3d(commandList, vbView, ibView, indicesSize);	//��
}

Object3d Motion::MovePadAndKey(Object3d object3d, float objSpeed)
{
	pad.Update();

	//�X�e�B�b�N�̌X�����擾
	stickVec = pad.GetLStick();
	//�傫������
	len = stickVec.length();
	len /= 32768;	//�l���ł������邩��0.0f~1.0f�ɂ���
	//���K��
	stickVec.normalize();

	//�X�e�B�b�N���|���ꂽ��
	if (len != 0)
	{
		//�ړ��ʂ����Z
		object3d.position.x += stickVec.x * len * objSpeed;
		object3d.position.z += stickVec.y * len * objSpeed;
	}

	if (key.IsKeyDown(DIK_UP) || key.IsKeyDown(DIK_DOWN) || key.IsKeyDown(DIK_RIGHT) || key.IsKeyDown(DIK_LEFT))
	{
		if (key.IsKeyDown(DIK_UP)) { object3d.position.y += 1.0f; }
		else if (key.IsKeyDown(DIK_DOWN)) { object3d.position.y -= 1.0f; }
		if (key.IsKeyDown(DIK_RIGHT)) { object3d.position.x += 1.0f; }
		else if (key.IsKeyDown(DIK_LEFT)) { object3d.position.x -= 1.0f; }
	}
	return object3d;
}

float Clamp(float value, float min, float max)
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