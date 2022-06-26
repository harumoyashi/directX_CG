#pragma once
#include "NDirectXInput.h"
#include "NObject.h"
#include "NEasing.h"

class Motion
{
private:
	float speedAmount = 0.05f;
	float speed = PI * speedAmount;	//移動速度係数&タイマー速度
	float timer = 0;				//モーションタイマー
	float halfTimer = 0;			//モーションタイマー(ハーフ)
	const float maxTimer = PI * 2;
	const float maxHalfTimer = PI;

	float rotSpeed;			//振る速度
	float easeInRotSpeed;	//イージング掛けたバージョン
	int easeSpeed = 4;		//イージングの加減
	float swingVec = 1.0f;	//腕や足を振る方向

	bool isMoveMode = true;	//true:run,false:walk

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
	//走りモード
	void RunMode();
	//歩きモード
	void WalkMode();

	float clamp(float value,float min,float max);
};

