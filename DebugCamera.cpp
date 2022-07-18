#include "DebugCamera.h"
#include <DirectXMath.h>
#include <WinUser.h>

void DebugCamera::DebugCameraIni(WNDCLASSEX w,HWND hwnd)
{
	input.Initialize(w,hwnd);
	eye = { 0, 0, -200 };	//視点座標
	target = { 0, 0, 0 };	//注視点座標
	up = { 0, 1, 0 };		//上方向ベクトル

	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	frontVec = { 0, 0, 0 };
	sideVec = { 0,0,0 };

	frontdist = 50;
}

void DebugCamera::Updata(HWND hwnd)
{
	//マウスの情報の更新
	input.GetState(hwnd);
	CameraMove();
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
}

void DebugCamera::CameraMove()
{
	//Vector3 proviUpVec = { 0,1,0 };
	//float speed = 0.5f;
	////カメラが注視点座標より奥にいるとき
	//if (up.y <= 0) {
	//	speed *= -1;
	//}

	////プレイヤーの正面ベクトル
	//frontVec = {
	//	target.x - eye.x,
	//	target.y - eye.y,
	//	target.z - eye.z
	//};
	//frontVec.normalize();

	//sideVec = proviUpVec.cross(frontVec);
	//sideVec.normalize();

	//upVec = sideVec.cross(frontVec);
	//upVec.normalize();

	////平行移動
	//if (input.IsMouseDown(MOUSE_WHEEL)) {
	//	//上下に動かしたとき
	//	if (input.GetCoursorMoveY() > 0) {
	//		cameraTrans -= upVec * speed;
	//		target.x -= upVec.x * speed;
	//		target.y -= upVec.y * speed;
	//		target.z -= upVec.z * speed;
	//	}
	//	else if (input.GetCoursorMoveY() < 0) {
	//		cameraTrans += upVec * speed;
	//		target.x += upVec.x * speed;
	//		target.y += upVec.y * speed;
	//		target.z += upVec.z * speed;
	//	}
	//	//マウスカーソルを左右に動かしたとき
	//	if (input.GetCoursorMoveX() > 0) {
	//		cameraTrans.x -= sideVec.x * speed;
	//		cameraTrans.z -= sideVec.z * speed;

	//		target.x -= sideVec.x * speed;
	//		target.z -= sideVec.z * speed;
	//	}
	//	else if (input.GetCoursorMoveX() < 0) {
	//		cameraTrans.x += sideVec.x * speed;
	//		cameraTrans.z += sideVec.z * speed;

	//		target.x += sideVec.x * speed;
	//		target.z += sideVec.z * speed;
	//	}

	//}
	////拡大縮小
	//else if (!input.IsMouseDown(MOUSE_WHEEL)) {
	//	frontdist += -input.IsMouseWheel() * (frontdist * 0.001f);
	//}
	//球面座標移動
	if (input.IsDown(0)) {
		//カメラが上を向いてるとき通常通りに座標を足す
		if (up.y >= 0) {
			moveDist += input.GetCursorVec() * 0.05f;
		}
		//カメラが逆さまになった時X.Z座標を反転させる
		else if (up.y <= 0) {
			moveDist.x -= input.GetCursorVec().x * 0.05f;
			moveDist.y += input.GetCursorVec().y * 0.05f;
			moveDist.z -= input.GetCursorVec().z * 0.05f;
		}
	}

	//カメラUP変換
	up = {
		0,
		cosf(moveDist.y),
		0
	};

	eye.x = -frontdist * sinf(moveDist.x) * cosf(moveDist.y);
	eye.y = frontdist * sinf(moveDist.y);
	eye.z = -frontdist * cosf(moveDist.x) * cosf(moveDist.y);
}

XMMATRIX DebugCamera::GetMatView()
{
	return matView;
}