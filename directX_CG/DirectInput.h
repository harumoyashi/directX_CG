#pragma once
#define DIRECTINPUT_VERSION  0x0800 //DirectInputのバージョン指定
#include <dinput.h>

void InputInitialize(HRESULT result, WNDCLASSEX w, HWND hwnd);
void KeybordUpdate(IDirectInputDevice8* keyboard);