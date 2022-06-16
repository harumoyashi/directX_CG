#pragma once
#include "NDirectX.h"

class Texture
{
private:
	HRESULT result;

public:
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	ScratchImage mipChain{};					//ミップマップ
	D3D12_RESOURCE_DESC textureResouceDesc{};	//リソース構造体
	ID3D12Resource* texBuff = nullptr;			//定数バッファ
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//設定構造体

	UINT incrementSize;	//次のテクスチャ情報の場所に進むために使う

public:
	//WICテクスチャのロード
	void Load(const wchar_t* pictureName);
	//ミップマップ生成
	void CreateMipmap();
	//リソース設定
	void SetResouce();
	//テクスチャバッファの生成
	void CreateTexBuff(ID3D12Device* device, D3D12_HEAP_PROPERTIES textureHeapProp);
	//ミップマップデータの転送
	void MipmapDataSend();
	//シェーダーリソースビュー作成
	void CreateSRV(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE srvHandle);
	//1つハンドルを進める
	void NextHandle(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE srvHandle);
};

const int maxTexture = 3;	//使用するテクスチャの枚数