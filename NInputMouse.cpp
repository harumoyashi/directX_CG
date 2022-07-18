#include "NInputMouse.h"
#include <cassert>

static IDirectInput8* directInput = nullptr;
static IDirectInputDevice8* mouse = nullptr;

void InputMouse::Initialize(WNDCLASSEX w, HWND hwnd)
{
	HRESULT result;
	// DirectInputの初期化
	result = DirectInput8Create(
		w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	// マウスデバイスの生成
	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	assert(SUCCEEDED(result));
	// 入力データ形式のセット
	result = mouse->SetDataFormat(&c_dfDIMouse); // 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = mouse->SetCooperativeLevel(
		//DISCL_FOREGROUND：画面が手前にある場合のみ入力を受け付ける
		//DISCL_NONEXCLUSIVE：デバイスをこのアプリだけで専有しない
		//DISCL_NOWINKEY：Windowsキーを無効にする
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void InputMouse::Update(HWND hwnd)
{
	GetState(hwnd);
	GetCursorPosition(hwnd);
}

void InputMouse::GetState(HWND hwnd)
{
	
	//前フレームの位置代入
	cursor.prevPos = cursor.nowPos;
	//マウス情報の取得開始
	mouse->Acquire();
	mouse->Poll();
	mouse->GetDeviceState(sizeof(DIMOUSESTATE),&state);
	//座標取得
	GetCursorPosition(hwnd);
}

void InputMouse::GetCursorPosition(HWND hwnd)
{
	//スクリーンから見たカーソル座標取得
	GetCursorPos(&cursor.p);
	//ウィンドウから見たカーソル座標取得
	ScreenToClient(hwnd, &cursor.p);

	//前フレームの状態を代入
	prev = state;
	//現フレームの座標代入
	cursor.nowPos.x = (float)cursor.p.x;
	cursor.nowPos.y = (float)cursor.p.y;
	//カーソルがどの方向に動いたかのベクトルを取得
	cursor.Vec = cursor.nowPos - cursor.prevPos;

	//正規化
	cursor.Vec.normalize();
}

bool InputMouse::IsDown(BYTE button)
{
	if (state.rgbButtons[button])
	{
		return true;
	}
	return false;
}

bool InputMouse::IsTrigger(BYTE button)
{
	if (!prev.rgbButtons[button]&&state.rgbButtons[button])
	{
		return true;
	}
	return false;
}

Vector3 InputMouse::GetCursorVec()
{
	return cursor.Vec;
}
