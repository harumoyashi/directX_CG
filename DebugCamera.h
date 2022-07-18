#pragma once
#include "NVector3.h"
#include "NInputMouse.h"
#include <DirectXMath.h>
using namespace DirectX;
class DebugCamera
{
public:
	void DebugCameraIni(WNDCLASSEX w,HWND hwnd);
	void Updata(HWND hwnd);


	XMMATRIX GetMatView();

private:
	void CameraMove();
	//ビュー変換行列
	XMMATRIX matView;
	XMFLOAT3 eye;		//視点座標
	XMFLOAT3 target;	//注視点座標
	XMFLOAT3 up;		//上方向ベクトル

	Vector3 dist = { 0,0,0 };
	Vector3 moveDist = {0,0,0};	//球面座標
	Vector3 cameraTrans = { 0,0,0 };//平行移動座標

	//正面・横・上ベクトル
	Vector3 frontVec;
	Vector3 sideVec;
	Vector3 upVec;

	InputMouse input;

	float frontdist;

};

