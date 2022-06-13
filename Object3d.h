#pragma once
#include "MyDirectX.h"

//親子構造番号割り当て
enum PartId
{
	kRoot,		//大元
	kSpine,		//脊椎
	kUpLegRootL,//左脚付け根
	kUpLegRootR,//右脚付け根
	kKneeL,		//左ひざ
	kKneeR,		//右ひざ
	kChest,		//胸
	kHead,		//頭
	kUpArmL,	//左二の腕
	kForeArmL,	//左前腕
	kHandL,		//左手
	kUpArmR,	//右二の腕
	kForeArmR,	//右前腕
	kHandR,		//右手
	kWaist,		//腰
	kHip,		//おしり
	kUpLegL,	//左大腿
	kLowLegL,	//左下腿
	kFootL,		//左足
	kUpLegR,	//右大腿
	kLowLegR,	//右下腿
	kFootR,		//右足

	kFloor,		//床

	kNumPartId	//構造体の数
};

class Object3d
{
public:
	//定数バッファ（行列用）
	ID3D12Resource* constBuffTransform;
	//定数バッファマップ（行列用）
	MyDirectX::ConstBufferDataTransform* constMapTransform;
	//アフィン変換情報
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 position = { 0,0,0 };
	//ワールド変換行列
	XMMATRIX matWorld;
	//親オブジェクトへのポインタ
	Object3d* parent = nullptr;

public:
	//初期化
	void Initialize(ID3D12Device* device);
	//更新
	void Update(XMMATRIX& matView, XMMATRIX& matProjection);
	//描画
	void Draw(ID3D12GraphicsCommandList* commandList,
		D3D12_VERTEX_BUFFER_VIEW& vbView, D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices);

	void SetRot();
};

