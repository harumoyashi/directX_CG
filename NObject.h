#pragma once
#include "NDirectX.h"

//3Dオブジェクト型
class Object3d
{
public:
	//定数バッファ（行列用）
	ComPtr<ID3D12Resource> constBuffTransform;
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
	void InitializeObject3d(ID3D12Device* device);
	//更新
	void UpdateObject3d(XMMATRIX matView, XMMATRIX& matProjection);
	//描画
	void DrawObject3d(ID3D12GraphicsCommandList* commandList,
		D3D12_VERTEX_BUFFER_VIEW& vbView, D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices);
};