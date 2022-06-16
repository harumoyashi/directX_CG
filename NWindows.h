#pragma once
#include <string>
#include <windows.h>
class Windows
{
public:
	const int width = 1280;
	const int height = 720;

	//ウィンドウクラスの生成＆登録
	WNDCLASSEX w{};
	//ウィンドウハンドルの生成
	HWND hwnd;
	//ウィンドウサイズ用の長方形の生成
	RECT wrc;
	//メッセージ情報構造体の生成
	MSG msg = {};

	//ウィンドウプロシージャ
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	//Windowクラスの設定
	void Set();
	//コンソールへの文字出力
	void DebugText(LPCSTR text);
	//ウィンドウオブジェクトの生成
	void CreateWindowObj();
	//ウィンドウ表示
	void Display();
};

