#pragma once
#include "NVector3.h"
#include "NInputMouse.h"
#include "NDirectXInput.h"
#include <DirectXMath.h>
using namespace DirectX;
class DebugCamera
{
public:
	void Initialize(WNDCLASSEX w, HWND hwnd);
	void Update(HWND hwnd);

	//ビュー変換行列取得
	XMMATRIX GetMatView();

private:
	//カメラ移動
	void CameraMove();

	XMMATRIX matView;	//ビュー変換行列
	XMFLOAT3 eye;		//視点座標
	XMFLOAT3 target;	//注視点座標
	XMFLOAT3 up;		//上方向ベクトル

	Vector3 move = { 0,0,0 };		//球面座標
	Vector3 mouseVec = { 0,0,0 };	//マウス動かした方向

	Vector3 eyeVec;
	Vector3 targetVec;

	Vector3 frontVec;	//正面ベクトル
	Vector3 rightVec;	//右ベクトル
	Vector3 upVec;		//上ベクトル
	Vector3 yVec = { 0,1,0 };	//Yの仮ベクトル(右ベクトル求めるのに使う)
	Vector3 trans = {0,0,0};		//カメラの移動量格納変数

	float rotSpeed = 0.05f;		//カメラの球面移動速度
	float transSpeed = 0.5f;	//カメラの平行移動速度

	//入力デバイス
	InputMouse mouse;
	DirectXInput key;
};

