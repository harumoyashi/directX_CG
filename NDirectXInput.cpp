#include "NDirectXInput.h"
#include <cassert>

#define DIRECTINPUT_VERSION 0x0800 // DirectInput�̃o�[�W�����w��
#include <dinput.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

// DirectInput�̏�����
static IDirectInput8* directInput = nullptr;
static IDirectInputDevice8* keyboard = nullptr;
static IDirectInputDevice8* pad = nullptr;

//LPDIRECTINPUT8       g_lpDI;
//LPDIRECTINPUTDEVICE8 g_lpDIDevice;
//DIDEVCAPS            g_diDevCaps;
//
//const DIDEVICEINSTANCE deviceInstance;
//const DIDEVICEOBJECTINSTANCE deviceObjInstance;
//
//DIJOYSTATE js;

// �S�L�[�̓��͏�Ԃ��擾����
static BYTE keys[256] = {};

// �S�L�[��1F�O�̓��͏�Ԃ��擾����
static BYTE prev[256] = {};

void DirectXInput::InputInit(WNDCLASSEX w, HWND hwnd)
{
	HRESULT result;

	// DirectInput�̏�����
	result = DirectInput8Create(
		w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	/*directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback,
		NULL, DIEDFL_ATTACHEDONLY);
	pad->SetDataFormat(&c_dfDIJoystick);
	pad->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	pad->GetCapabilities(&g_diDevCaps);
	pad->EnumObjects(EnumAxesCallback, (void*)hwnd, DIDFT_AXIS);
	result = g_lpDIDevice->Poll();
	if (FAILED(result)) {
		result = g_lpDIDevice->Acquire();
		while (result == DIERR_INPUTLOST) {
			result = g_lpDIDevice->Acquire();
		}
	}*/

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
}

void DirectXInput::InputUpdate()
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


	/*g_lpDIDevice->Poll();
	g_lpDIDevice->GetDeviceState(sizeof(DIJOYSTATE), &js);*/
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

//bool DirectXInput::IsButtonDown()
//{
//	if (js.rgbButtons[0] & 0 * 80)
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}
//
//BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
//{
//	HRESULT result;
//
//	result = directInput->CreateDevice(pdidInstance->guidInstance, &pad, NULL);
//
//	if (FAILED(result)) return DIENUM_CONTINUE;
//
//	return DIENUM_STOP;
//}
//
//BOOL CALLBACK EnumAxesCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext)
//{
//	HRESULT     result;
//	DIPROPRANGE diprg;
//
//	diprg.diph.dwSize = sizeof(DIPROPRANGE);
//	diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
//	diprg.diph.dwHow = DIPH_BYID;
//	diprg.diph.dwObj = pdidoi->dwType;
//	diprg.lMin = 0 - 1000;	//���E�l
//	diprg.lMax = 0 + 1000;
//	result = pad->SetProperty(DIPROP_RANGE, &diprg.diph);
//
//	//���s������
//	if (FAILED(result)) return DIENUM_STOP;
//	//����������
//	return DIENUM_CONTINUE;
//}