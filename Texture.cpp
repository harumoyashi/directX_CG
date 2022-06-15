#include "Texture.h"

void Texture::Initialize()
{
	//						x		y		z		u		v
	vertices.push_back({ { -50.0f, -50.0f, 0.0f }, {0.0f,1.0f} });	// 左下
	vertices.push_back({ { -50.0f, 50.0f , 0.0f }, {0.0f,0.0f} });	// 左上
	vertices.push_back({ { 50.0f , -50.0f, 0.0f }, {1.0f,1.0f} });	// 右下
	vertices.push_back({ { 50.0f , 50.0f , 0.0f }, {1.0f,0.0f} });	// 右上
}

void Texture::Lord(const wchar_t* name_format)
{
	result = LoadFromWICFile(
		name_format,
		WIC_FLAGS_NONE,
		&metadate, scratchImg);
}

void Texture::CreateMipMap()
{
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadate = scratchImg.GetMetadata();
	}
}

void Texture::SetSRGB()
{
	metadate.format = MakeSRGB(metadate.format);
}

void Texture::SetTexBuff()
{
	//テクスチャバッファ設定
	//ヒープ設定
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//リソース設定
	textureResouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResouceDesc.Format = metadate.format;
	textureResouceDesc.Width = metadate.width;	//幅
	textureResouceDesc.Height = (UINT)metadate.height;	//高さ
	textureResouceDesc.DepthOrArraySize = (UINT16)metadate.arraySize;
	textureResouceDesc.MipLevels = (UINT16)metadate.mipLevels;
	textureResouceDesc.SampleDesc.Count = 1;
}

void Texture::CreateTexBuff(ID3D12Device* device)
{
	//テクスチャバッファの生成
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResouceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);
}

void Texture::SetMipMap()
{
	//全ミップマップについて
	for (size_t i = 0; i < metadate.mipLevels; i++)
	{
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		result = texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,				//全領域へコピー
			img->pixels,			//元データアドレス
			(UINT)img->rowPitch,	//1ラインサイズ
			(UINT)img->slicePitch	//全サイズ
		);
		assert(SUCCEEDED(result));
	}
}
