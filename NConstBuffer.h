#pragma once
#include "NDirectX.h"

#pragma region 構造体宣言
//定数バッファ用構造体
struct ConstBufferDataMaterial
{
	XMFLOAT4 color;	//色(RGBA)
};

class ConstBuff
{
public:
	ID3D12Resource* constBuffMaterial = nullptr;
	D3D12_HEAP_PROPERTIES cbHeapProp{};		//ヒープ設定
	D3D12_RESOURCE_DESC cbResouceDesc{};	//リソース設定
	ConstBufferDataMaterial* constMapMaterial = nullptr;	//定数バッファのマップ

private:


public:
	//ヒープ設定
	void SetHeapProp();
	//リソース設定
	void SetResoucedesc();
	//定数バッファの生成
	void Create(ID3D12Device* device);
	//定数バッファのマッピング
	void Mapping();

	//値を書き込むと自動的に転送される
	void TransfarColor(float R, float G, float B, float A);
};