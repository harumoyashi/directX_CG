#include "NDirectXInput.h"
#include <cassert>

#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定
#include <dinput.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

// DirectInputの初期化
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

// 全キーの入力状態を取得する
static BYTE keys[256] = {};

// 全キーの1F前の入力状態を取得する
static BYTE prev[256] = {};

void DirectXInput::InputInit(WNDCLASSEX w, HWND hwnd)
{
	HRESULT result;

	// DirectInputの初期化
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
}

void DirectXInput::InputUpdate()
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


	/*g_lpDIDevice->Poll();
	g_lpDIDevice->GetDeviceState(sizeof(DIJOYSTATE), &js);*/
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
//	diprg.lMin = 0 - 1000;	//限界値
//	diprg.lMax = 0 + 1000;
//	result = pad->SetProperty(DIPROP_RANGE, &diprg.diph);
//
//	//失敗したら
//	if (FAILED(result)) return DIENUM_STOP;
//	//成功したら
//	return DIENUM_CONTINUE;
//}