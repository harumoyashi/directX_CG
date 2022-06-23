#pragma once
#include "NDirectXInput.h"
#include "NObject.h"
#include "NEasing.h"

class Motion
{
private:
	float speed = 0.05f;	//移動速度係数&タイマー速度
	float timer = 0;		//モーションタイマー
	float halfTimer = 0;	//モーションタイマー(ハーフ)
	const float maxTimer = PI*2;
	const float maxHalfTimer = PI;

	float rotSpeed;
	float legRotSpeed;

	DirectXInput key;	//キーボード

public:
	//3Dオブジェクトの配列
	Object3d object3d[kNumPartId];

	Object3d floor;

public:
	//初期化
	void Initialize(ID3D12Device* device);
	//更新
	void Update(XMMATRIX matView, XMMATRIX matProjection);
	//描画
	void Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW vbView, D3D12_INDEX_BUFFER_VIEW ibView, int indicesSize);
	//エンター押してる間タイマー進む
	void StartTimer();
	//キー入力回転処理
	void RotationKey();
};

