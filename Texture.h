#pragma once
#include "MyDirectX.h"

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

public:
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

