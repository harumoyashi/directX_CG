#pragma once
#include "Vector3.h"
#include <Windows.h>
#include <array>
#include <vector>
#include <wrl.h>

#include <XInput.h>
#define DIRECTINPUT_VERSION 0x0800 // DirectInput�̃o�[�W�����w��
#include <dinput.h>

class DirectXInput
{
public:
	enum class PadType {
		DirectInput,
		XInput,
	};

	// variant��C++17����
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

	//input������
	static void Initialize(WNDCLASSEX w, HWND hwnd);
	//input�X�V
	static void Update();

	//�L�[�{�[�h���͏����p (�Ԃ�l0,1)
	//�������ςȂ�
	static bool IsKeyDown(UINT8 key);	//UINT�̓r�b�g���w�肵����^���R�ۂ�
	//�������u��
	static bool IsKeyTrigger(UINT8 key);
	//�����Ă�Ƃ�
	static bool GetKeyReleased(UINT8 key);
	//�������u��
	static bool GetKeyReleaseTrigger(UINT8 key);

	//�p�b�h���͏����p
	//�������ςȂ�
	static bool IsButtonDown(UINT8 pad);	//UINT�̓r�b�g���w�肵����^���R�ۂ�
	//�������u��
	static bool IsButtonTrigger(UINT8 pad);

private:
	static BOOL CALLBACK
		EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) noexcept;
	/*DirectXInput() = default;
	~DirectXInput();
	DirectXInput(const DirectXInput&) = delete;
	const DirectXInput& operator=(const DirectXInput&) = delete;*/
	void SetupJoysticks();

private: // �����o�ϐ�
	Microsoft::WRL::ComPtr<IDirectInput8> dInput_;
	std::vector<Joystick> devJoysticks_;
	HWND hwnd_;
};