#pragma once
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
	static bool IsKeyDown(char key);
	//�������u��
	static bool IsKeyTrigger(char key);
	//�����Ă�Ƃ�
	static bool GetKeyReleased(char key);
	//�������u��
	static bool GetKeyReleaseTrigger(char key);
};