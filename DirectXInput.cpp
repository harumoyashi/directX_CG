#include "DirectXInput.h"
#include <cassert>

#include <XInput.h>
#include <basetsd.h>
#include <commctrl.h>
#include <dbt.h>
#include <oleauto.h>
#include <shellapi.h>
#include <wbemidl.h>
#include <wrl/client.h>

#include <Windows.h>
#include <dinputd.h>
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

namespace
{
	std::vector<DWORD> sXInputVidPids;
	IDirectInputDevice8* sCurrentDevice = nullptr;
}

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
	// XInput����
	SetupForIsXInputDevice();

	// JoyStick�̃Z�b�g�A�b�v
	SetupJoysticks();

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


HRESULT SetupForIsXInputDevice() noexcept {
	struct bstr_deleter {
		void operator()(void* p) noexcept { SysFreeString(static_cast<BSTR>(p)); }
	};
	using ScopedBSTR = std::unique_ptr<OLECHAR[], bstr_deleter>;

	std::vector<DWORD> sXInputVidPids;
	sXInputVidPids.clear();
	// COM needs to be initialized on this thread before the enumeration.

	// So we can call VariantClear() later, even if we never had a successful
	// IWbemClassObject::Get().
	VARIANT var = {};
	VariantInit(&var);

	// Create WMI
	Microsoft::WRL::ComPtr<IWbemLocator> pIWbemLocator;
	HRESULT hr = CoCreateInstance(
		__uuidof(WbemLocator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pIWbemLocator));
	if (FAILED(hr) || pIWbemLocator == nullptr) {
		return hr;
	}

	// Create BSTRs for WMI
	ScopedBSTR bstrNamespace(SysAllocString(L"\\\\.\\root\\cimv2"));
	ScopedBSTR bstrClassName(SysAllocString(L"Win32_PNPEntity"));
	ScopedBSTR bstrDeviceID(SysAllocString(L"DeviceID"));
	if (!bstrNamespace || !bstrClassName || !bstrDeviceID) {
		return E_OUTOFMEMORY;
	}

	// Connect to WMI
	Microsoft::WRL::ComPtr<IWbemServices> pIWbemServices;
	hr = pIWbemLocator->ConnectServer(
		bstrNamespace.get(), nullptr, nullptr, 0L, 0L, nullptr, nullptr, &pIWbemServices);
	if (FAILED(hr) || pIWbemServices == nullptr) {
		return hr;
	}

	// Switch security level to IMPERSONATE
	hr = CoSetProxyBlanket(
		pIWbemServices.Get(), RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
	if (FAILED(hr)) {
		return hr;
	}

	// Get list of Win32_PNPEntity devices
	Microsoft::WRL::ComPtr<IEnumWbemClassObject> pEnumDevices;
	hr = pIWbemServices->CreateInstanceEnum(bstrClassName.get(), 0, nullptr, &pEnumDevices);
	if (FAILED(hr) || pEnumDevices == nullptr) {
		return hr;
	}

	// Loop over all devices
	IWbemClassObject* pDevices[20] = {};
	for (;;) {
		ULONG uReturned = 0;
		hr = pEnumDevices->Next(10000, _countof(pDevices), pDevices, &uReturned);
		if (FAILED(hr)) {
			return hr;
		}

		if (uReturned == 0) {
			break;
		}

		assert(uReturned <= _countof(pDevices));
		_Analysis_assume_(uReturned <= _countof(pDevices));

		for (size_t iDevice = 0; iDevice < uReturned; ++iDevice) {
			if (!pDevices[iDevice]) {
				continue;
			}

			// For each device, get its device ID
			hr = pDevices[iDevice]->Get(bstrDeviceID.get(), 0L, &var, nullptr, nullptr);
			if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != nullptr) {
				// Check if the device ID contains "IG_".  If it does, then it's an XInput device
				// Unfortunately this information can not be found by just using DirectInput
				if (wcsstr(var.bstrVal, L"IG_")) {
					// If it does, then get the VID/PID from var.bstrVal
					DWORD dwPid = 0, dwVid = 0;
					const WCHAR* strVid = wcsstr(var.bstrVal, L"VID_");
					if (strVid && swscanf_s(strVid, L"VID_%4X", &dwVid) != 1) {
						dwVid = 0;
					}
					const WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
					if (strPid && swscanf_s(strPid, L"PID_%4X", &dwPid) != 1) {
						dwPid = 0;
					}

					const DWORD dwVidPid = MAKELONG(dwVid, dwPid);

					// Add the VID/PID to a linked list
					sXInputVidPids.push_back(dwVidPid);
				}
			}

			VariantClear(&var);
			SAFE_RELEASE(pDevices[iDevice]);
		}
	}

	VariantClear(&var);

	for (size_t iDevice = 0; iDevice < _countof(pDevices); ++iDevice) {
		SAFE_RELEASE(pDevices[iDevice]);
	}

	return hr;
}

bool IsXInputDevice(const GUID* pGuidProductFromDirectInput) noexcept {
	for (DWORD vidPid : sXInputVidPids) {
		if (vidPid == pGuidProductFromDirectInput->Data1) {
			return true;
		}
	}
	return false;
}

BOOL CALLBACK EnumAxesCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext) {
	if (!sCurrentDevice) {
		return DIENUM_STOP;
	}

	DIPROPRANGE diprg;
	diprg.diph.dwSize = sizeof(DIPROPRANGE);
	diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	diprg.diph.dwHow = DIPH_BYID;
	diprg.diph.dwObj = pdidoi->dwType;
	diprg.lMin = -32768;
	diprg.lMax = 32767;
	HRESULT hr = sCurrentDevice->SetProperty(DIPROP_RANGE, &diprg.diph);

	if (FAILED(hr)) {
		return DIENUM_STOP;
	}
	return DIENUM_CONTINUE;
}

BOOL CALLBACK
DirectXInput::EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) noexcept {
	DirectXInput* input = static_cast<DirectXInput*>(pContext);
	Microsoft::WRL::ComPtr<IDirectInputDevice8> joystick;
	HRESULT hr = input->dInput_->CreateDevice(pdidInstance->guidInstance, &joystick, nullptr);
	if (FAILED(hr)) {
		return DIENUM_CONTINUE;
	}

	DIDEVICEINSTANCE instance;
	joystick->GetDeviceInfo(&instance);
	bool isXInput = IsXInputDevice(&pdidInstance->guidProduct);
	input->devJoysticks_.push_back(Joystick{
	  joystick, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
	  isXInput ? PadType::XInput : PadType::DirectInput });

	std::memset(&input->devJoysticks_.back().state_, 0, sizeof(State));

	return DIENUM_CONTINUE;
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

		sCurrentDevice = joystick.device_.Get();
		joystick.device_->EnumObjects(EnumAxesCallback, reinterpret_cast<void*>(hwnd_), DIDFT_AXIS);
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
