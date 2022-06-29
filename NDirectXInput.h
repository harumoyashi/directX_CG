#pragma once
#include <dinput.h>

class DirectXInput
{
public:
	//input初期化
	static void InputInit(WNDCLASSEX w, HWND hwnd);
	//input更新
	static void InputUpdate();

	//キーボード入力処理用 (返り値0,1)
	//押しっぱなし
	static bool IsKeyDown(UINT8 key);	//UINTはビット数指定したら型自由ぽい
	//押した瞬間
	static bool IsKeyTrigger(UINT8 key);
	//離してるとき
	static bool GetKeyReleased(UINT8 key);
	//離した瞬間
	static bool GetKeyReleaseTrigger(UINT8 key);

	//static bool IsButtonDown();
	////コールバック関数
	//BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
	//BOOL CALLBACK EnumAxesCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext);
};