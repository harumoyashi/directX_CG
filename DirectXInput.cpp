#include "DirectXInput.h"
#include <cassert>

//#include <XInput.h>
//#include <basetsd.h>
//#include <commctrl.h>
//#include <dbt.h>
//#include <oleauto.h>
//#include <shellapi.h>
//#include <wbemidl.h>
//#include <wrl/client.h>

//#include <Windows.h>
//#include <dinputd.h>
#include <memory>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "xinput.lib")

#define SAFE_RELEASE(p)                                                                            \
	{                                                                                              \
		if (p) {                                                                                   \
			(p)->Release();                                                                        \
			(p) = nullptr;                                                                         \
		}                                                                                          \
	}

//namespace
//{
//	std::vector<DWORD> sXInputVidPids;
//	IDirectInputDevice8* sCurrentDevice = nullptr;
//}

// DirectInput�̏�����
static IDirectInput8* directInput = nullptr;
static IDirectInputDevice8* keyboard = nullptr;
static IDirectInputDevice8* pad = nullptr;

// �S�L�[�̓��͏�Ԃ��擾����
static BYTE keys[256] = {};
// �S�L�[��1F�O�̓��͏�Ԃ��擾����
static BYTE prev[256] = {};

//// �S�p�b�h�̓��͏�Ԃ��擾����
//static BYTE pads[256] = {};
//// �S�p�b�h��1F�O�̓��͏�Ԃ��擾����
//static BYTE prevPads[256] = {};

DirectXInput* DirectXInput::GetInstance()
{
	static DirectXInput instance;
	return &instance;
}

void DirectXInput::Initialize(WNDCLASSEX w, HWND hwnd)
{
	HRESULT result;

	// DirectInput�̏�����
	result = DirectInput8Create(
		w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	// �L�[�{�[�h�f�o�C�X�̐���
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	// ���̓f�[�^�`���̃Z�b�g
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); // �W���`��
	assert(SUCCEEDED(result));

	// �r�����䃌�x���̃Z�b�g
	result = keyboard->SetCooperativeLevel(
		//DISCL_FOREGROUND�F��ʂ���O�ɂ���ꍇ�̂ݓ��͂��󂯕t����
		//DISCL_NONEXCLUSIVE�F�f�o�C�X�����̃A�v�������Ő�L���Ȃ�
		//DISCL_NOWINKEY�FWindows�L�[�𖳌��ɂ���
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	//�p�b�h�̏�����//
	

	//// �p�b�h�f�o�C�X�̐���
	//result = directInput->CreateDevice(GUID_SysKeyboard, &pad, NULL);
	//assert(SUCCEEDED(result));
}

void DirectXInput::Update()
{
	// �S�L�[��1F�O�̓��͏�Ԃ��擾����
	keyboard->GetDeviceState(sizeof(prev), prev);

	// �O��X�V
	for (int i = 0; i < 256; ++i)
	{
		prev[i] = keys[i];
	}

	// �L�[�{�[�h���̎擾�J�n
	keyboard->Acquire();
	keyboard->GetDeviceState(sizeof(keys), keys);

}

//�������ςȂ�
bool DirectXInput::IsKeyDown(UINT8 key)
{
	return keys[key];
}

//�������u��
bool DirectXInput::IsKeyTrigger(UINT8 key)
{
	return keys[key] && !prev[key];
}

//�����Ă�Ƃ�
bool DirectXInput::GetKeyReleased(UINT8 key)
{
	return !keys[key] && !prev[key];
}

//�������u��
bool DirectXInput::GetKeyReleaseTrigger(UINT8 key)
{
	return !keys[key] && prev[key];
}




void DirectXInput::SetupJoysticks()
{
	devJoysticks_.clear();
	// JoyStick�̗�
	HRESULT result =
		dInput_->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
	assert(SUCCEEDED(result));

	// ���̓f�[�^�`���E�r�����䃌�x���̃Z�b�g
	for (auto& joystick : devJoysticks_) {
		result = joystick.device_->SetDataFormat(&c_dfDIJoystick2);
		assert(SUCCEEDED(result));
		result =
			joystick.device_->SetCooperativeLevel(hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		assert(SUCCEEDED(result));
		joystick.deadZoneL_ = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		joystick.deadZoneR_ = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		// ���̃��[�h���Ύ��ɐݒ�
		DIPROPDWORD diprop;
		diprop.diph.dwSize = sizeof(DIPROPDWORD);
		diprop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		diprop.diph.dwHow = DIPH_DEVICE;
		diprop.diph.dwObj = 0;
		diprop.dwData = DIPROPAXISMODE_ABS;
		joystick.device_->SetProperty(DIPROP_AXISMODE, &diprop.diph);

		/*sCurrentDevice = joystick.device_.Get();*/
	}
}

////�{�^��
//bool DirectXInput::IsButtonDown(UINT8 pad)
//{
//	return pads[pad];
//}
//
//bool DirectXInput::IsButtonTrigger(UINT8 pad)
//{
//	return pads[pad] && !prevPads[pad];
//}
