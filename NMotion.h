#pragma once
#include "NDirectXInput.h"
#include "NInputPad.h"
#include "NObject.h"
#include "NEasing.h"

class Motion
{
	struct Speed
	{
		float spd;
		const float min;
		const float max;
		float spdAmount = (max - min) * 0.1f;
	};

private:
	Speed speedAmount = { 0.02f,0.02f,0.1f };
	float speed = PI * speedAmount.spd;	//移動速度係数&タイマー速度
	float timer = 0;				//モーションタイマー
	float halfTimer = 0;			//モーションタイマー(ハーフ)
	const float maxTimer = PI * 2;
	const float maxHalfTimer = PI;

	float rotSpeed;			//振る速度
	float easeInRotSpeed;	//イージング掛けたバージョン
	Speed easeSpeed = { 2.0f,2.0f,4.0f };	//イージングの加減
	float swingVec = 1.0f;	//腕や足を振る方向

	float angle = 0.0f;

	const float spdNorm = 4.0f;

	//各部位の回転スピード設定
	Speed upArmSpd = { 0.5f,0.5f,1.2f };
	Speed foreArmSpd = { 0.2f,0.2f,0.3f };
	Speed upLegSpd = { 0.4f,0.4f,1.0f };
	Speed kneeSpd = { 0.5f,0.5f,0.8f };
	Speed footSpd = { 0.1f,0.1f,0.2f };
	Speed centroidSpd = { 0.7f,0.7f,4.0f };
	Speed bodyTiltSpd = { 0.02f,0.02f,0.05f };
	Speed chestTwistSpd = { 0.1f,0.1f,0.2f };
	Speed elbowPlusSpd = { 0.1f,0.1f,1.2f };
	Speed kneePlusSpd = { 0.6f,0.6f,1.2f };

	float rotVec = -1.0f;	//true:run,false:walk

	DirectXInput key;	//キーボード
	InputPad pad;

	Vector2 stickVec = {};
	float len;
	Vector2 frontVec = { 0,1 };

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
	//座標操作
	void Operation();
	//回転処理
	void RotUpdate();

	//閾値超えないように
	float Clamp(float value, float min, float max);
	//スピード専用Clamp
	float SpeedClamp(Speed Spd);
};

