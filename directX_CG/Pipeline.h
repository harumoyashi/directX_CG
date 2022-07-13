#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <d3d12.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
using namespace DirectX;

class Pipeline
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

	ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
	ID3DBlob* errorBlob = nullptr; // エラーオブジェクト

	//頂点レイアウト
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;

	// グラフィックスパイプラインステート
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

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

	// 頂点シェーダの読み込みとコンパイル
	void VSCompile();
	// ピクセルシェーダの読み込みとコンパイル
	void PSCompile();
	// 頂点レイアウト
	void CreateVerLayout();

	// シェーダーの設定
	void SetShader();
	// ラスタライザの設定
	void SetRasterizer();
	// ブレンドステート設定
	void SetBlendState();
	// 頂点レイアウトの設定
	void SetVerLayout();
	// 図形の形状設定
	void SetTopology();
};

