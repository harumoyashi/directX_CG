#pragma once
#include <dinput.h>
#include "NVector3.h"

class InputMouse
{
public:
	void Initialize(HWND hwnd);
	void Update();

	void GetState(HWND hwnd);
	void GetCursorPosition(HWND hwnd);

	bool IsTrigger(BYTE button);

private:
	DIMOUSESTATE state;
	DIMOUSESTATE prev;

	//�J�[�\���\����
	struct Cursor
	{
		POINT p;
		Vector3 nowPos;
		Vector3 prevPos;
		Vector3 Vec;
	};
};

