#include "NMotion.h"

void Motion::Initialize(ID3D12Device* device)
{
	object3d.InitializeObject3d(device);
}

void Motion::Update(XMMATRIX matView, XMMATRIX matProjection)
{
}

void Motion::Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW vbView, D3D12_INDEX_BUFFER_VIEW ibView, int indicesSize)
{
	object3d.DrawObject3d(commandList, vbView, ibView, indicesSize);
	floor.DrawObject3d(commandList, vbView, ibView, indicesSize);	//床
}

Object3d Motion::MovePadAndKey(Object3d object3d, float objSpeed)
{
	pad.Update();

	//スティックの傾きを取得
	stickVec = pad.GetLStick();
	//大きさを代入
	len = stickVec.length();
	len /= 32768;	//値がでかすぎるから0.0f~1.0fにする
	//正規化
	stickVec.normalize();

	//スティックが倒された時
	if (len != 0)
	{
		//移動量を加算
		object3d.position.x += stickVec.x * len * objSpeed;
		object3d.position.z += stickVec.y * len * objSpeed;
	}

	if (key.IsKeyDown(DIK_UP) || key.IsKeyDown(DIK_DOWN) || key.IsKeyDown(DIK_RIGHT) || key.IsKeyDown(DIK_LEFT))
	{
		if (key.IsKeyDown(DIK_UP)) { object3d.position.y += 1.0f; }
		else if (key.IsKeyDown(DIK_DOWN)) { object3d.position.y -= 1.0f; }
		if (key.IsKeyDown(DIK_RIGHT)) { object3d.position.x += 1.0f; }
		else if (key.IsKeyDown(DIK_LEFT)) { object3d.position.x -= 1.0f; }
	}
	return object3d;
}

float Clamp(float value, float min, float max)
{
	if (value < min)
	{
		return min;
	}
	else if (value > max)
	{
		return max;
	}
	return value;
}