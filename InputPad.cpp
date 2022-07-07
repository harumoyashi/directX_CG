#include "InputPad.h"

static BYTE buttons[16];
static BYTE prev[16];

void InputPad::Initialize()
{
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	DWORD result;
	result = XInputGetState(0, &state);

	if (result == ERROR_SUCCESS)
	{
		isConnect = true;
	}
	else
	{
		isConnect = false;
	}
}

void InputPad::Update()
{
	XInputGetState(0, &state);
	//// 前後更新
	//for (int i = 0; i < 16; ++i)
	//{
	//	prev[i] = buttons[i];
	//}

	////buttonsの更新
}

bool InputPad::IsButtonDown(UINT button)
{
	if (state.Gamepad.wButtons & button)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool InputPad::IsButtonTrigger(UINT button)
{
	return false;
}

bool InputPad::IsButtonReleased(UINT button)
{
	return false;
}

bool InputPad::IsButtonReleaseTrigger(UINT button)
{
	return false;
}

Vector2 InputPad::GetLStick(SHORT sThumbLX, SHORT sThumbLY)
{
	int x = sThumbLX;
	int y = sThumbLY;

	return Vector2(x,y);
}
