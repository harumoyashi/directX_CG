#include "NWindows.h"

//ウィンドプロシージャ
//面倒だけど書かなきゃいけない関数
LRESULT Windows::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		//ウィンドウが破棄されたら呼ばれる
	case WM_DESTROY:
		PostQuitMessage(0);	//OSに対して「もうこのアプリは終わる」と伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);	//既定の処理を行う
}

//Windowクラスの設定
void Windows::Set()
{
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;		//ウィンドウプロシージャを設定
	w.lpszClassName = L"DX12Sample";			//アプリケーションクラス名
	w.hInstance = GetModuleHandle(nullptr);		//ハンドルの取得
	w.hCursor = LoadCursor(NULL, IDC_ARROW);	//カーソル指定

	RegisterClassEx(&w);	//アプリケーションクラス（ウィンドウクラスの指定をOSに伝える）
}

//コンソールへの文字出力
void Windows::DebugText(LPCSTR text)
{
	OutputDebugStringA(text);
}

//ウィンドウオブジェクトの生成
void Windows::CreateWindowObj()
{
	wrc = { 0,0,width,height };	//ウィンドウサイズを決める

	//関数を使ってウィンドウのサイズを補正する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウオブジェクトの生成
	hwnd = CreateWindow(
		w.lpszClassName,		//クラス名指定
		L"DX12テスト",			//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	//タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT,			//表示x座標はOSにお任せ
		CW_USEDEFAULT,			//表示y座標はOSにお任せ
		wrc.right - wrc.left,	//ウィンドウ幅
		wrc.bottom - wrc.top,	//ウィンドウ高
		nullptr,				//親ウィンドウハンドル
		nullptr,				//メニューハンドル
		w.hInstance,			//呼び出しアプリケーションハンドル
		nullptr					//追加パラメーター
	);
}

//ウィンドウ表示
void Windows::Display()
{
	ShowWindow(hwnd, SW_SHOW);
}