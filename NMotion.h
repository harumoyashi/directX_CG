#pragma once
#include "NDirectXInput.h"
#include "NObject.h"
#include "NEasing.h"

class Motion
{
private:
	//現在のスピード、最大値、最小値、加速度
	struct Speed
	{
		float spd;
		const float min;
		const float max;
		const float spdAmount = (max - min) / 10.0f;
	};

	Speed speedAmount = {0.02f,0.02f,0.1f};
	float speed = PI * speedAmount.spd;	//移動速度係数&タイマー速度
	float timer = 0;				//モーションタイマー
	float halfTimer = 0;			//モーションタイマー(ハーフ)
	const float maxTimer = PI * 2;
	const float maxHalfTimer = PI;

	float rotSpeed;			//振る速度
	float easeInRotSpeed;	//イージング掛けたバージョン
	Speed easeSpeed = { 2.0f,2.0f,4.0f };	//イージングの加減
	float swingVec = 1.0f;	//腕や足を振る方向

	float angle = 0.0f;	//ぐるぐる走らせる用の角度

	//各部位の回転スピード設定
	Speed upArmSpd =      { 0.5f,0.5f,1.2f };
	Speed foreArmSpd =    { 0.2f,0.2f,0.3f };
	Speed upLegSpd =      { 0.4f,0.4f,1.0f };
	Speed kneeSpd =       { 0.5f,0.5f,0.8f };
	Speed footSpd =       { 0.1f,0.1f,0.2f };
	Speed centroidSpd =   { 0.7f,0.7f,4.0f };
	Speed bodyTiltSpd =   { 0.02f,0.02f,0.05f };
	Speed chestTwistSpd = { 0.1f,0.1f,0.2f };
	Speed elbowPlusSpd =  { 0.1f,0.1f,1.2f };
	Speed kneePlusSpd =   { 0.6f,0.6f,1.2f };

	float rotVec = -1.0f;	//true:run,false:walk

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

