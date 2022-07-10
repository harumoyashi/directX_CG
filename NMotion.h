#pragma once
#include "NDirectXInput.h"
#include "NInputPad.h"
#include "NObject.h"

class Motion
{
private:
	DirectXInput key;	//キーボード
	InputPad pad;

	Vector2 frontVec = { 0,1 };

public:
	//3Dオブジェクトの配列
	Object3d object3d;

	Object3d floor;

public:
	//初期化
	void Initialize(ID3D12Device* device);
	//更新
	void Update(XMMATRIX matView, XMMATRIX matProjection);
	//描画
	void Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW vbView, D3D12_INDEX_BUFFER_VIEW ibView, int indicesSize);
	
	//閾値超えないように
	float Clamp(float value, float min, float max);
};
