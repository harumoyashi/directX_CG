#pragma once
#include "NWindows.h"
#include <DirectXTex.h>

#include <d3d12.h>
#include <dxgi1_6.h>	//Visual Studio Graphics Debuggerが誤動作起こしたら、1_4にすると解決するかも

#include <DirectXMath.h>
using namespace DirectX;

#include "NDirectXInput.h"

class MyDirectX
{
public:
	HRESULT result;

	//デバッグレイヤーをオンにするために使用されるインターフェイス
	ID3D12Debug* debugController;

	//定数バッファ用データ構造体（3D変換行列）
	struct  ConstBufferDataTransform
	{
		XMMATRIX mat;	//3D変換行列
	};


	//定数バッファ0番
	ID3D12Resource* constBuffTransform0 = nullptr;
	ConstBufferDataTransform* constMapTransform0 = nullptr;
	//定数バッファ1番
	ID3D12Resource* constBuffTransform1 = nullptr;
	ConstBufferDataTransform* constMapTransform1 = nullptr;

	//基本オブジェクトの生成
	//受け皿になる変数生成

	//色々なもの作成するための仮想アダプタ
	ID3D12Device* device = nullptr;
	//アダプターの列挙状態の変化を検出できるようにするためのインターフェース
	IDXGIFactory7* dxgiFactory = nullptr;
	//ダブルバッファリングのために画面切り替え用のバッファー管理するやつ
	IDXGISwapChain4* swapChain = nullptr;
	//コマンドリストに格納する命令の為のメモリを管理するオブジェクト
	ID3D12CommandAllocator* commandAllocator = nullptr;
	//命令を一時的に格納しとくやつ
	ID3D12GraphicsCommandList* commandList = nullptr;
	//GPUが実行するべき命令のリストを,指定した順序でGPUに転送するためのインターフェース
	ID3D12CommandQueue* commandQueue = nullptr;

	ID3D12DescriptorHeap* rtvHeap = nullptr;

	ConstBufferDataTransform* constMapTransform = nullptr;
	//定数バッファのGPUリソースのポインタ
	ID3D12Resource* constBuffTransform = nullptr;

	//アダプターの列挙用
	std::vector<IDXGIAdapter4*> adapters;

	//ここに特定の名前を持つアダプターオブジェクトが入る
	IDXGIAdapter4* tmpAdapter = nullptr;

	D3D_FEATURE_LEVEL featureLevel;

	//コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};

	ID3D12DescriptorHeap* srvHeap = nullptr;

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};

	std::vector<ID3D12Resource*> backBuffers;

	//シェーダーリソースビューハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;
	//レンダーターゲットビューハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	//レンダーターゲットビューの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	//DirectX初期化
	void Init(HWND hwnd);

private:
	//ウィンドウクラス
	Windows win;

	//アダプター選択
	void ChoiceAdapters();
	//デバイスの生成
	void CreateDevice();
	//コマンドアロケータ、リスト、キューの生成
	void CreateCommandGroup();
	// スワップチェーンの生成
	void CreateSwapChain(HWND hwnd);
	//シェーダリソースビューのデスクリプタヒープ生成
	void CreateSRVHeapDesc();
	//レンダーターゲットビューのデスクリプタヒープ生成
	void CreateRTVHeapDesc();
};

