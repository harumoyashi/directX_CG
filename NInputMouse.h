#pragma once
#include <dinput.h>
#include "NVector3.h"

enum Mouse
{
	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_WHEEL,
};

class InputMouse
{
public:
	void Initialize(WNDCLASSEX w, HWND hwnd);
	void Update(HWND hwnd);

	void GetState(HWND hwnd);
	void GetCursorPosition(HWND hwnd);

	bool IsDown(BYTE button);
	bool IsTrigger(BYTE button);

	Vector3 GetCursorVec();

private:
	DIMOUSESTATE state;
	DIMOUSESTATE prev;

	//カーソル構造体
	struct Cursor
	{
		POINT p;
		Vector3 nowPos;
		Vector3 prevPos;
		Vector3 Vec;
	};

	Cursor cursor;
};

