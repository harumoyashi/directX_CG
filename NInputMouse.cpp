#include "NInputMouse.h"
#include <cassert>

static IDirectInput8* directInput = nullptr;
static IDirectInputDevice8* mouse = nullptr;

void InputMouse::Initialize(HWND hwnd)
{
	HRESULT result;
	// �}�E�X�f�o�C�X�̐���
	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	assert(SUCCEEDED(result));
	// ���̓f�[�^�`���̃Z�b�g
	result = mouse->SetDataFormat(&c_dfDIMouse); // �W���`��
	assert(SUCCEEDED(result));

	// �r�����䃌�x���̃Z�b�g
	result = mouse->SetCooperativeLevel(
		//DISCL_FOREGROUND�F��ʂ���O�ɂ���ꍇ�̂ݓ��͂��󂯕t����
		//DISCL_NONEXCLUSIVE�F�f�o�C�X�����̃A�v�������Ő�L���Ȃ�
		//DISCL_NOWINKEY�FWindows�L�[�𖳌��ɂ���
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void InputMouse::Update()
{

}

void InputMouse::GetState(HWND hwnd)
{
	//�O�t���[���̏�Ԃ���
	prev = state;
	//�}�E�X���̎擾�J�n
	mouse->Acquire();
	mouse->Poll();
	mouse->GetDeviceState(sizeof(DIMOUSESTATE),&state);

	//���W�擾
	GetCursorPosition(hwnd);
}

void InputMouse::GetCursorPosition(HWND hwnd)
{
	Cursor cursor;
	//�X�N���[�����猩���J�[�\�����W�擾
	GetCursorPos(&cursor.p);
	//�E�B���h�E���猩���J�[�\�����W�擾
	ScreenToClient(hwnd, &cursor.p);

	//�O�t���[���̈ʒu���
	cursor.prevPos = cursor.nowPos;
	//���t���[���̍��W���
	cursor.nowPos.x = (float)cursor.p.x;
	cursor.nowPos.y = (float)cursor.p.y;

	//�J�[�\�����ǂ̕����ɓ��������̃x�N�g�����擾
	cursor.Vec = cursor.nowPos - cursor.prevPos;
	//���K��
	cursor.Vec.normalize();
}

bool InputMouse::IsTrigger(BYTE button)
{
	if (!prev.rgbButtons[button]&&state.rgbButtons[button])
	{
		return true;
	}
	return false;
}
