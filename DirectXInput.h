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
	static bool IsKeyDown(char key);
	//押した瞬間
	static bool IsKeyTrigger(char key);
	//離してるとき
	static bool GetKeyReleased(char key);
	//離した瞬間
	static bool GetKeyReleaseTrigger(char key);
};