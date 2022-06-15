#pragma once
#include "MyDirectX.h"
#include <DirectXMath.h>

class Texture
{
public:
	HRESULT result;

	TexMetadata metadate{};
	ScratchImage scratchImg{};
	ScratchImage mipChain{};

	D3D12_HEAP_PROPERTIES textureHeapProp{};
	D3D12_RESOURCE_DESC textureResouceDesc{};
	ID3D12Resource* texBuff = nullptr;

	//頂点データ構造体
	struct Vertex
	{
		XMFLOAT3 pos;	//xyz座標
		XMFLOAT2 uv;	//uv座標
	};

	std::vector<Vertex> vertices;

public:
	//初期化
	void Initialize();

	//WICテクスチャのロード
	void Lord(const wchar_t* name_format);
	//ミップマップ生成
	void CreateMipMap();
	//読み込んだディフューズテクスチャをSRGBとして扱う
	void SetSRGB();

	void SetTexBuff();
	void CreateTexBuff(ID3D12Device* device);
	//全ミップマップについて
	void SetMipMap();
};

