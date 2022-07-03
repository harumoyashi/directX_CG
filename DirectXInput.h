#pragma once
#include "Vector3.h"
#include <Windows.h>
#include <array>
#include <vector>
#include <wrl.h>

#include <XInput.h>
#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定
#include <dinput.h>

class DirectXInput
{
public:
	enum class PadType {
		DirectInput,
		XInput,
	};

	// variantがC++17から
	union State {
		XINPUT_STATE xInput_;
		DIJOYSTATE2 directInput_;
	};

	struct Joystick {
		Microsoft::WRL::ComPtr<IDirectInputDevice8> device_;
		int32_t deadZoneL_;
		int32_t deadZoneR_;
		PadType type_;
		State state_;
		State statePre_;
	};

public:
	static DirectXInput* GetInstance();

	//input初期化
	static void Initialize(WNDCLASSEX w, HWND hwnd);
	//input更新
	static void Update();

	//キーボード入力処理用 (返り値0,1)
	//押しっぱなし
	static bool IsKeyDown(UINT8 key);	//UINTはビット数指定したら型自由ぽい
	//押した瞬間
	static bool IsKeyTrigger(UINT8 key);
	//離してるとき
	static bool GetKeyReleased(UINT8 key);
	//離した瞬間
	static bool GetKeyReleaseTrigger(UINT8 key);

	//パッド入力処理用
	//押しっぱなし
	static bool IsButtonDown(UINT8 pad);	//UINTはビット数指定したら型自由ぽい
	//押した瞬間
	static bool IsButtonTrigger(UINT8 pad);

private:
	static BOOL CALLBACK
		EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) noexcept;
	/*DirectXInput() = default;
	~DirectXInput();
	DirectXInput(const DirectXInput&) = delete;
	const DirectXInput& operator=(const DirectXInput&) = delete;*/
	void SetupJoysticks();

private: // メンバ変数
	Microsoft::WRL::ComPtr<IDirectInput8> dInput_;
	std::vector<Joystick> devJoysticks_;
	HWND hwnd_;
};