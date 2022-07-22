#include "NDebugCamera.h"
#include <DirectXMath.h>
#include <WinUser.h>

void DebugCamera::Initialize(WNDCLASSEX w, HWND hwnd)
{
	mouse.Initialize(w, hwnd);
	eye = { 0, 0, cameraLen };	//���_���W
	target = { 0, 0, 0 };	//�����_���W
	up = { 0, 1, 0 };		//������x�N�g��
	move = { 0,0,0 };
	mouseVec = { 0,0,0 };
	eyeVec = { 0,0,0 };
	targetVec = { 0,0,0 };
	trans = { 0,0,0 };

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
	//�J�����̕��s�ړ�//
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
	//��x�N�g���擾
	upVec = rightVec.cross(frontVec);
	upVec.normalize();

	//���s�ړ����x�ݒ�
	transSpeed = 0.5f;
	if (up.y <= 0)
	{
		transSpeed *= -1;
	}

	if (mouse.IsDown(MOUSE_WHEEL))
	{
		//���E�ړ�
		if (mouse.GetCursorVec().x > 0)
		{
			trans.x -= rightVec.x * transSpeed;
			trans.z -= rightVec.z * transSpeed;

			targetVec.x -= rightVec.x * transSpeed;
			targetVec.z -= rightVec.z * transSpeed;
		}
		else if (mouse.GetCursorVec().x < 0)
		{
			trans.x += rightVec.x * transSpeed;
			trans.z += rightVec.z * transSpeed;

			targetVec.x += rightVec.x * transSpeed;
			targetVec.z += rightVec.z * transSpeed;
		}
		//�㉺�ړ�
		if (mouse.GetCursorVec().y > 0)
		{
			trans -= upVec * transSpeed;
			targetVec -= upVec * transSpeed;
		}
		else if (mouse.GetCursorVec().y < 0)
		{
			trans += upVec * transSpeed;
			targetVec += upVec * transSpeed;
		}
	}
	else
	{
		//�z�C�[�������ĂȂ��Ƃ�
		//�z�C�[����]�ɉ����Ċg��k��
		cameraLen += -mouse.GetWheel() * (cameraLen * 0.001f);
	}
	//�����_���W�ƃJ�������W��XMFLOAT3�^�ɖ߂�
	eye = { eyeVec.x,eyeVec.y,eyeVec.z };
	target = { targetVec.x,targetVec.y,targetVec.z };

	//�J�����̋��ʍ��W�ړ�//
	if (mouse.IsDown(MOUSE_LEFT)) {
		//�J��������������Ă�Ƃ��ʏ�ʂ�ɍ��W�𑫂�
		if (up.y >= 0) {
			move += mouse.GetCursorVec() * rotSpeed;
		}
		//�J�������t���܂ɂȂ�����X.Z���W�𔽓]������
		else if (up.y <= 0) {
			move.x -= mouse.GetCursorVec().x * rotSpeed;
			move.y += mouse.GetCursorVec().y * rotSpeed;
			move.z -= mouse.GetCursorVec().z * rotSpeed;
		}
	}

	//�J����UP�ϊ�
	up = { 0,cosf(move.y),0 };
	//�J�������W�ɑ��
	eye.x = -cameraLen * sinf(move.x) * cosf(move.y) + trans.x;
	eye.y = cameraLen * sinf(move.y) + trans.y;
	eye.z = -cameraLen * cosf(move.x) * cosf(move.y) + trans.z;
}

XMMATRIX DebugCamera::GetMatView()
{
	return matView;
}