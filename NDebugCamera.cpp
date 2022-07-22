#include "NDebugCamera.h"
#include <DirectXMath.h>
#include <WinUser.h>

void DebugCamera::Initialize(WNDCLASSEX w, HWND hwnd)
{
	mouse.Initialize(w, hwnd);
	eye = { 0, 0, cameraLen };	//視点座標
	target = { 0, 0, 0 };	//注視点座標
	up = { 0, 1, 0 };		//上方向ベクトル
	move = { 0,0,0 };
	mouseVec = { 0,0,0 };
	eyeVec = { 0,0,0 };
	targetVec = { 0,0,0 };
	trans = { 0,0,0 };

	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	frontVec = { 0, 0, 0 };
}

void DebugCamera::Update(HWND hwnd)
{
	//マウスの情報の更新
	mouse.GetState(hwnd);
	CameraMove();
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
}

void DebugCamera::CameraMove()
{
	//カメラの平行移動//
	//注視点座標とカメラ座標をVector3型に格納
	eyeVec = { eye.x,eye.y,eye.z };
	targetVec = { target.x,target.y,target.z };

	//注視点座標とカメラ座標から正面ベクトル取得
	frontVec = targetVec - eyeVec;
	//正面ベクトル正規化
	frontVec.normalize();
	//右ベクトル取得
	rightVec = yVec.cross(frontVec);
	rightVec.normalize();
	//上ベクトル取得
	upVec = rightVec.cross(frontVec);
	upVec.normalize();

	//平行移動速度設定
	transSpeed = 0.5f;
	if (up.y <= 0)
	{
		transSpeed *= -1;
	}

	if (mouse.IsDown(MOUSE_WHEEL))
	{
		//左右移動
		if (mouse.GetCursorVec().x > 0)
		{
			trans.x -= rightVec.x * transSpeed;
			trans.z -= rightVec.z * transSpeed;

			targetVec.x -= rightVec.x * transSpeed;
			targetVec.z -= rightVec.z * transSpeed;
		}
		else if (mouse.GetCursorVec().x < 0)
		{
			trans.x += rightVec.x * transSpeed;
			trans.z += rightVec.z * transSpeed;

			targetVec.x += rightVec.x * transSpeed;
			targetVec.z += rightVec.z * transSpeed;
		}
		//上下移動
		if (mouse.GetCursorVec().y > 0)
		{
			trans -= upVec * transSpeed;
			targetVec -= upVec * transSpeed;
		}
		else if (mouse.GetCursorVec().y < 0)
		{
			trans += upVec * transSpeed;
			targetVec += upVec * transSpeed;
		}
	}
	else
	{
		//ホイール押してないとき
		//ホイール回転に応じて拡大縮小
		cameraLen += -mouse.GetWheel() * (cameraLen * 0.001f);
	}
	//注視点座標とカメラ座標をXMFLOAT3型に戻す
	eye = { eyeVec.x,eyeVec.y,eyeVec.z };
	target = { targetVec.x,targetVec.y,targetVec.z };

	//カメラの球面座標移動//
	if (mouse.IsDown(MOUSE_LEFT)) {
		//カメラが上を向いてるとき通常通りに座標を足す
		if (up.y >= 0) {
			move += mouse.GetCursorVec() * rotSpeed;
		}
		//カメラが逆さまになった時X.Z座標を反転させる
		else if (up.y <= 0) {
			move.x -= mouse.GetCursorVec().x * rotSpeed;
			move.y += mouse.GetCursorVec().y * rotSpeed;
			move.z -= mouse.GetCursorVec().z * rotSpeed;
		}
	}

	//カメラUP変換
	up = { 0,cosf(move.y),0 };
	//カメラ座標に代入
	eye.x = -cameraLen * sinf(move.x) * cosf(move.y) + trans.x;
	eye.y = cameraLen * sinf(move.y) + trans.y;
	eye.z = -cameraLen * cosf(move.x) * cosf(move.y) + trans.z;
}

XMMATRIX DebugCamera::GetMatView()
{
	return matView;
}