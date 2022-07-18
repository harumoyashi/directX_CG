#pragma once
#include "NVector3.h"
#include "NInputMouse.h"
#include <DirectXMath.h>
using namespace DirectX;
class DebugCamera
{
public:
	void DebugCameraIni(WNDCLASSEX w,HWND hwnd);
	void Updata(HWND hwnd);


	XMMATRIX GetMatView();

private:
	void CameraMove();
	//�r���[�ϊ��s��
	XMMATRIX matView;
	XMFLOAT3 eye;		//���_���W
	XMFLOAT3 target;	//�����_���W
	XMFLOAT3 up;		//������x�N�g��

	Vector3 dist = { 0,0,0 };
	Vector3 moveDist = {0,0,0};	//���ʍ��W
	Vector3 cameraTrans = { 0,0,0 };//���s�ړ����W

	//���ʁE���E��x�N�g��
	Vector3 frontVec;
	Vector3 sideVec;
	Vector3 upVec;

	InputMouse input;

	float frontdist;

};

