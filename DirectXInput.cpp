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

// DirectInputの初期化
static IDirectInput8* directInput = nullptr;
static IDirectInputDevice8* keyboard = nullptr;
static IDirectInputDevice8* pad = nullptr;

// 全キーの入力状態を取得する
static BYTE keys[256] = {};
// 全キーの1F前の入力状態を取得する
static BYTE prev[256] = {};

//// 全パッドの入力状態を取得する
//static BYTE pads[256] = {};
//// 全パッドの1F前の入力状態を取得する
//static BYTE prevPads[256] = {};

DirectXInput* DirectXInput::GetInstance()
{
	static DirectXInput instance;
	return &instance;
}

void DirectXInput::Initialize(WNDCLASSEX w, HWND hwnd)
{
	HRESULT result;

	// DirectInputの初期化
	result = DirectInput8Create(
		w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	// キーボードデバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	// 入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		//DISCL_FOREGROUND：画面が手前にある場合のみ入力を受け付ける
		//DISCL_NONEXCLUSIVE：デバイスをこのアプリだけで専有しない
		//DISCL_NOWINKEY：Windowsキーを無効にする
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	//パッドの初期化//
	

	//// パッドデバイスの生成
	//result = directInput->CreateDevice(GUID_SysKeyboard, &pad, NULL);
	//assert(SUCCEEDED(result));
}

void DirectXInput::Update()
{
	// 全キーの1F前の入力状態を取得する
	keyboard->GetDeviceState(sizeof(prev), prev);

	// 前後更新
	for (int i = 0; i < 256; ++i)
	{
		prev[i] = keys[i];
	}

	// キーボード情報の取得開始
	keyboard->Acquire();
	keyboard->GetDeviceState(sizeof(keys), keys);

}

//押しっぱなし
bool DirectXInput::IsKeyDown(UINT8 key)
{
	return keys[key];
}

//押した瞬間
bool DirectXInput::IsKeyTrigger(UINT8 key)
{
	return keys[key] && !prev[key];
}

//離してるとき
bool DirectXInput::GetKeyReleased(UINT8 key)
{
	return !keys[key] && !prev[key];
}

//離した瞬間
bool DirectXInput::GetKeyReleaseTrigger(UINT8 key)
{
	return !keys[key] && prev[key];
}




void DirectXInput::SetupJoysticks()
{
	devJoysticks_.clear();
	// JoyStickの列挙
	HRESULT result =
		dInput_->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
	assert(SUCCEEDED(result));

	// 入力データ形式・排他制御レベルのセット
	for (auto& joystick : devJoysticks_) {
		result = joystick.device_->SetDataFormat(&c_dfDIJoystick2);
		assert(SUCCEEDED(result));
		result =
			joystick.device_->SetCooperativeLevel(hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		assert(SUCCEEDED(result));
		joystick.deadZoneL_ = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		joystick.deadZoneR_ = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		// 軸のモードを絶対軸に設定
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

////ボタン
//bool DirectXInput::IsButtonDown(UINT8 pad)
//{
//	return pads[pad];
//}
//
//bool DirectXInput::IsButtonTrigger(UINT8 pad)
//{
//	return pads[pad] && !prevPads[pad];
//}
