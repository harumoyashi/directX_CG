#pragma once
#include "NDirectXInput.h"
#include "NObject.h"

class Motion
{
private:
	int timer = 0;	//モーションタイマー
	const int maxTimer = 300;

	DirectXInput key;	//キーボード

public:
	//3Dオブジェクトの配列
	Object3d object3d[kNumPartId];

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

