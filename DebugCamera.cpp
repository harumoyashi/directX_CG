#include "DebugCamera.h"
#include <DirectXMath.h>
#include <WinUser.h>

void DebugCamera::Initialize(WNDCLASSEX w, HWND hwnd)
{
	mouse.Initialize(w, hwnd);
	eye = { 0, 0, -100 };	//���_���W
	target = { 0, 0, 0 };	//�����_���W
	up = { 0, 1, 0 };		//������x�N�g��

	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	frontVec = { 0, 0, 0 };
}

void DebugCamera::Update(HWND hwnd)
{
	//�}�E�X�̏��̍X�V
	mouse.GetState(hwnd);
	CameraMove();
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
}

void DebugCamera::CameraMove()
{
	//�}�E�X�ŃJ�����̋��ʍ��W�ړ�//
	if (mouse.IsDown(MOUSE_LEFT)) {
		//�J��������������Ă�Ƃ��ʏ�ʂ�ɍ��W�𑫂�
		if (up.y >= 0) {
			move += mouse.GetCursorVec() * mouseSpeed;
		}
		//�J�������t���܂ɂȂ�����X.Z���W�𔽓]������
		else if (up.y <= 0) {
			move.x -= mouse.GetCursorVec().x * mouseSpeed;
			move.y += mouse.GetCursorVec().y * mouseSpeed;
			move.z -= mouse.GetCursorVec().z * mouseSpeed;
		}
	//�J����UP�ϊ�
	up = { 0,cosf(move.y),0 };
	//�J�������W�ɑ��
	eye.x = -100.0f * sinf(move.x) * cosf(move.y);
	eye.y = 100.0f * sinf(move.y);
	eye.z = -100.0f * cosf(move.x) * cosf(move.y);
	}


	//�L�[�ŃJ�����̕��s�ړ�//
	//�����_���W�ƃJ�������W��Vector3�^�Ɋi�[
	eyeVec = { eye.x,eye.y,eye.z };
	targetVec = { target.x,target.y,target.z };

	//�����_���W�ƃJ�������W���琳�ʃx�N�g���擾
	frontVec = targetVec - eyeVec;
	//���ʃx�N�g�����K��
	frontVec.normalize();

	//�E�x�N�g���擾
	rightVec = yVec.cross(frontVec);
	rightVec.normalize();

	if (key.IsKeyDown(DIK_A))
	{
		eyeVec += rightVec * moveSpeed;
		targetVec += rightVec * moveSpeed;
	}
	else if (key.IsKeyDown(DIK_D))
	{
		eyeVec -= rightVec * moveSpeed;
		targetVec -= rightVec * moveSpeed;
	}

	if (key.IsKeyDown(DIK_W))
	{
		eyeVec += frontVec * moveSpeed;
		targetVec += frontVec * moveSpeed;
	}
	else if (key.IsKeyDown(DIK_S))
	{
		eyeVec -= frontVec * moveSpeed;
		targetVec -= frontVec * moveSpeed;
	}

	//�����_���W�ƃJ�������W��XMFLOAT3�^�ɖ߂�
	eye = { eyeVec.x,eyeVec.y,eyeVec.z };
	target = { targetVec.x,targetVec.y,targetVec.z };
}

XMMATRIX DebugCamera::GetMatView()
{
	return matView;
}