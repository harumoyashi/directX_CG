#include "DebugCamera.h"
#include <DirectXMath.h>
#include <WinUser.h>

void DebugCamera::DebugCameraIni(WNDCLASSEX w,HWND hwnd)
{
	input.Initialize(w,hwnd);
	eye = { 0, 0, -200 };	//���_���W
	target = { 0, 0, 0 };	//�����_���W
	up = { 0, 1, 0 };		//������x�N�g��

	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	frontVec = { 0, 0, 0 };
	sideVec = { 0,0,0 };

	frontdist = 50;
}

void DebugCamera::Updata(HWND hwnd)
{
	//�}�E�X�̏��̍X�V
	input.GetState(hwnd);
	CameraMove();
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
}

void DebugCamera::CameraMove()
{
	//Vector3 proviUpVec = { 0,1,0 };
	//float speed = 0.5f;
	////�J�����������_���W��艜�ɂ���Ƃ�
	//if (up.y <= 0) {
	//	speed *= -1;
	//}

	////�v���C���[�̐��ʃx�N�g��
	//frontVec = {
	//	target.x - eye.x,
	//	target.y - eye.y,
	//	target.z - eye.z
	//};
	//frontVec.normalize();

	//sideVec = proviUpVec.cross(frontVec);
	//sideVec.normalize();

	//upVec = sideVec.cross(frontVec);
	//upVec.normalize();

	////���s�ړ�
	//if (input.IsMouseDown(MOUSE_WHEEL)) {
	//	//�㉺�ɓ��������Ƃ�
	//	if (input.GetCoursorMoveY() > 0) {
	//		cameraTrans -= upVec * speed;
	//		target.x -= upVec.x * speed;
	//		target.y -= upVec.y * speed;
	//		target.z -= upVec.z * speed;
	//	}
	//	else if (input.GetCoursorMoveY() < 0) {
	//		cameraTrans += upVec * speed;
	//		target.x += upVec.x * speed;
	//		target.y += upVec.y * speed;
	//		target.z += upVec.z * speed;
	//	}
	//	//�}�E�X�J�[�\�������E�ɓ��������Ƃ�
	//	if (input.GetCoursorMoveX() > 0) {
	//		cameraTrans.x -= sideVec.x * speed;
	//		cameraTrans.z -= sideVec.z * speed;

	//		target.x -= sideVec.x * speed;
	//		target.z -= sideVec.z * speed;
	//	}
	//	else if (input.GetCoursorMoveX() < 0) {
	//		cameraTrans.x += sideVec.x * speed;
	//		cameraTrans.z += sideVec.z * speed;

	//		target.x += sideVec.x * speed;
	//		target.z += sideVec.z * speed;
	//	}

	//}
	////�g��k��
	//else if (!input.IsMouseDown(MOUSE_WHEEL)) {
	//	frontdist += -input.IsMouseWheel() * (frontdist * 0.001f);
	//}
	//���ʍ��W�ړ�
	if (input.IsDown(0)) {
		//�J��������������Ă�Ƃ��ʏ�ʂ�ɍ��W�𑫂�
		if (up.y >= 0) {
			moveDist += input.GetCursorVec() * 0.05f;
		}
		//�J�������t���܂ɂȂ�����X.Z���W�𔽓]������
		else if (up.y <= 0) {
			moveDist.x -= input.GetCursorVec().x * 0.05f;
			moveDist.y += input.GetCursorVec().y * 0.05f;
			moveDist.z -= input.GetCursorVec().z * 0.05f;
		}
	}

	//�J����UP�ϊ�
	up = {
		0,
		cosf(moveDist.y),
		0
	};

	eye.x = -frontdist * sinf(moveDist.x) * cosf(moveDist.y);
	eye.y = frontdist * sinf(moveDist.y);
	eye.z = -frontdist * cosf(moveDist.x) * cosf(moveDist.y);
}

XMMATRIX DebugCamera::GetMatView()
{
	return matView;
}