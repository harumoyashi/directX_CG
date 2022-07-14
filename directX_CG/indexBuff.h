#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <d3d12.h>
using namespace DirectX;
class IndexBuff
{
public:
	// 頂点データ
	std::vector<XMFLOAT3>vertices;
	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB;

	D3D12_HEAP_PROPERTIES heapProp{}; // ヒープ設定
	// リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	// 頂点バッファ
	ID3D12Resource* vertBuff = nullptr;
	//頂点データコピー用
	XMFLOAT3* vertMap = nullptr;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};

public:
	void Initialize(ID3D12Device* device);
	void Update();

private:
	//頂点データ設定
	void SetVertices();
	//頂点バッファ設定
	void SetVerBuff();
	// 頂点バッファの生成
	void CreateVerBuff(ID3D12Device* device);
	//頂点バッファへのデータ転送
	void TransferVerBuff();
	// 頂点バッファビューの作成
	void CreateVerBuffView();
};

