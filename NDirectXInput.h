#pragma once
#define DIRECTINPUT_VERSION 0x0800 // DirectInput�̃o�[�W�����w��
#include <dinput.h>

class DirectXInput
{
public:
	//input������
	static void InputInit(WNDCLASSEX w, HWND hwnd);
	//input�X�V
	static void InputUpdate();

	//�L�[�{�[�h���͏����p (�Ԃ�l0,1)
	//�������ςȂ�
	static bool IsKeyDown(UINT8 key);	//UINT�̓r�b�g���w�肵����^���R�ۂ�
	//�������u��
	static bool IsKeyTrigger(UINT8 key);
	//�����Ă�Ƃ�
	static bool GetKeyReleased(UINT8 key);
	//�������u��
	static bool GetKeyReleaseTrigger(UINT8 key);
};