#include "NInputPad.h"

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
    SetDeadZone();
    //// 前後更新
    //for (int i = 0; i < 16; ++i)
    //{
    //    prev[i] = buttons[i];
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

void InputPad::SetDeadZone()
{
    if ((state.Gamepad.sThumbLX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
        state.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
        (state.Gamepad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
            state.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
    {
        state.Gamepad.sThumbLX = 0;
        state.Gamepad.sThumbLY = 0;
    }
}

Vector2 InputPad::GetLStick()
{
    SHORT x = state.Gamepad.sThumbLX;
    SHORT y = state.Gamepad.sThumbLY;

    return Vector2(static_cast<float>(x), static_cast<float>(y));
}