#pragma once
#define DIRECTINPUT_VERSION  0x0800 //DirectInput�̃o�[�W�����w��
#include <dinput.h>

void InputInitialize(HRESULT result, WNDCLASSEX w, HWND hwnd);
void KeybordUpdate(IDirectInputDevice8* keyboard);