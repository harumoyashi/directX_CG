#pragma once
#include "NVector3.h"
#include "NInputMouse.h"
#include "NDirectXInput.h"
#include <DirectXMath.h>
using namespace DirectX;
class DebugCamera
{
public:
	void Initialize(WNDCLASSEX w, HWND hwnd);
	void Update(HWND hwnd);

	//�r���[�ϊ��s��擾
	XMMATRIX GetMatView();

private:
	//�J�����ړ�
	void CameraMove();

	XMMATRIX matView;	//�r���[�ϊ��s��
	XMFLOAT3 eye;		//���_���W
	XMFLOAT3 target;	//�����_���W
	XMFLOAT3 up;		//������x�N�g��

	Vector3 move = { 0,0,0 };		//���ʍ��W
	Vector3 mouseVec = { 0,0,0 };	//�}�E�X������������

	Vector3 eyeVec;
	Vector3 targetVec;

	Vector3 frontVec;	//���ʃx�N�g��
	Vector3 rightVec;	//�E�x�N�g��
	Vector3 upVec;		//��x�N�g��
	Vector3 yVec = { 0,1,0 };	//Y�̉��x�N�g��(�E�x�N�g�����߂�̂Ɏg��)
	Vector3 trans = {0,0,0};		//�J�����̈ړ��ʊi�[�ϐ�

	float rotSpeed = 0.05f;		//�J�����̋��ʈړ����x
	float transSpeed = 0.5f;	//�J�����̕��s�ړ����x

	//���̓f�o�C�X
	InputMouse mouse;
	DirectXInput key;
};

