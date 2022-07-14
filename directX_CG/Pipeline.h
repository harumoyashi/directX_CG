#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <d3d12.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
using namespace DirectX;

//ブレンドモード識別番号
enum BlendMode
{
	noBlend,
	add,
	sub,
	invers,
	alpha,
};

class Pipeline
{
public:
	ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
	ID3DBlob* errorBlob = nullptr; // エラーオブジェクト

	//頂点レイアウト
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;

	// グラフィックスパイプライン
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

public:
	void Initialize(ID3D12Device* device, int blendMode);
	void Update();

private:
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
	void SetBlendState(int blendMode);
	// 頂点レイアウトの設定
	void SetVerLayout();
	// 図形の形状設定
	void SetTopology();
};

