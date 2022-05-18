#include "MyWindows.h"
#include "MyDirectX.h"
#include <vector>
#include <string>

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include "DirectXInput.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#pragma region 構造体宣言
//定数バッファ用構造体
struct ConstBufferDataMaterial
{
	XMFLOAT4 color;	//色(RGBA)
};
#pragma endregion

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#pragma region WindowsAPI初期化
	Windows win;	//ウィンドウクラス
	win.Set();
	win.CreateWindowObj();
	win.Display();
#pragma endregion
#pragma region DirectX初期化
	//DirectX初期化ここから
	HRESULT result;
	MyDirectX directX;
	directX.Init(win.hwnd);

//input初期化
DirectXInput keyboard;
keyboard.InputInit(result, win.w, win.hwnd);

	//DirectX初期化ここまで
#pragma endregion
#pragma region 描画初期化処理
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResouceDesc{};
	cbResouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResouceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;	//256バイトアラインメント
	cbResouceDesc.Height = 1;
	cbResouceDesc.DepthOrArraySize = 1;
	cbResouceDesc.MipLevels = 1;
	cbResouceDesc.SampleDesc.Count = 1;
	cbResouceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ID3D12Resource* constBuffMaterial = nullptr;
	//定数バッファの生成
	result = directX.device->CreateCommittedResource(
		&cbHeapProp,	//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResouceDesc,	//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial)
	);
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);	//マッピング
	assert(SUCCEEDED(result));

	{
		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPUへの転送用
		//リソース設定
		D3D12_RESOURCE_DESC cbResouceDesc{};
		cbResouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResouceDesc.Width = (sizeof(directX.constMapTransform) + 0xff) & ~0xff;	//256バイトアラインメント
		cbResouceDesc.Height = 1;
		cbResouceDesc.DepthOrArraySize = 1;
		cbResouceDesc.MipLevels = 1;
		cbResouceDesc.SampleDesc.Count = 1;
		cbResouceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		ID3D12Resource* constBuffMaterial = nullptr;
		//定数バッファの生成
		result = directX.device->CreateCommittedResource(
			&cbHeapProp,	//ヒープ設定
			D3D12_HEAP_FLAG_NONE,
			&cbResouceDesc,	//リソース設定
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&directX.constBuffTransform)
		);
		assert(SUCCEEDED(result));

		//定数バッファのマッピング
		result = directX.constBuffTransform->Map(0, nullptr, (void**)&directX.constMapTransform);	//マッピング
		assert(SUCCEEDED(result));
	}

	/*XMMATRIX oldVer = XMMatrixIdentity();
	oldVer.r[0].m128_f32[0] = 2.0f / window_width;
	oldVer.r[1].m128_f32[1] = -2.0f / window_height;

	oldVer.r[3].m128_f32[0] = -1.0f;
	oldVer.r[3].m128_f32[1] = 1.0f;

	XMMATRIX newVer = XMMatrixOrthographicOffCenterLH(
		0,window_width,
		window_height,0,
		0.0f, 1.0f
	);*/

	//平行投影変換//
	//単位行列を代入
	directX.constMapTransform->mat = XMMatrixIdentity();

	/*constMapTransform->mat.r[0].m128_f32[0] = 2.0f / window_width;
	constMapTransform->mat.r[1].m128_f32[1] = -2.0f / window_height;

	constMapTransform->mat.r[3].m128_f32[0] = -1.0f;
	constMapTransform->mat.r[3].m128_f32[1] = 1.0f;*/

	//平行投影変換
	directX.constMapTransform->mat = XMMatrixOrthographicOffCenterLH(
		0, win.width,
		win.height, 0,
		0.0f, 1.0f
	);

	//投資投影変換//


	//値を書き込むと自動的に転送される
	constMapMaterial->color = XMFLOAT4(1, 0, 0, 0.5f);	//RGBAで半透明の赤

	//デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;		//一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;	//テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	//定数バッファ0番
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//定数バッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0;					//定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダーから見える
	//テクスチャレジスタ0番
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//種類
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;			//デスクリプタレンジ
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						//デスクリプタレンジ数
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//全てのシェーダーから見える
	//定数バッファ1番
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[2].Descriptor.ShaderRegister = 1;					//定数バッファ番号
	rootParams[2].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダーから見える

	////横方向ピクセル数
	//const size_t textureWidth = 256;
	////縦方向ピクセル数
	//const size_t textureHeight = 256;
	////配列の要素数
	//const size_t imageDataCount = textureWidth * textureHeight;
	////画像イメージデータ配列
	//XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];	//※必ず後で解放する

	////全ピクセルの色を初期化
	//for (size_t i = 0; i < imageDataCount; i++)
	//{
	//	imageData[i].x = 1.0f;	//R
	//	imageData[i].y = 0.0f;	//G
	//	imageData[i].z = 0.0f;	//B
	//	imageData[i].w = 1.0f;	//A
	//}

	TexMetadata metadate{};
	ScratchImage scratchImg{};
	//WICテクスチャのロード
	result = LoadFromWICFile(
		L"Resources/itiro_kimegao.png",
		WIC_FLAGS_NONE,
		&metadate, scratchImg);

	ScratchImage mipChain{};
	//ミップマップ生成
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadate = scratchImg.GetMetadata();
	}

	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadate.format = MakeSRGB(metadate.format);

	//頂点データ構造体
	struct Vertex
	{
		XMFLOAT3 pos;	//xyz座標
		XMFLOAT2 uv;	//uv座標
	};

	// 頂点データ
	Vertex vertices[] = {
		//	x		y		z		u	v
		{{ 0.0f  , 100.0f, 0.0f }, {0.0f,1.0f}},	// 左下
		{{ 0.0f  , 0.0f  , 0.0f }, {0.0f,0.0f}},	// 左上
		{{ 100.0f, 100.0f, 0.0f }, {1.0f,1.0f}},	// 右下
		{{ 100.0f, 0.0f  , 0.0f }, {1.0f,0.0f}},	// 右上
	};

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	// 頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{}; // ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用

	// リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // 頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 頂点バッファの生成
	ID3D12Resource* vertBuff = nullptr;
	result = directX.device->CreateCommittedResource(
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// 全頂点に対して
	for (int i = 0; i < _countof(vertices); i++) {
		vertMap[i] = vertices[i]; // 座標をコピー
	}
	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);

	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	// GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	vbView.StrideInBytes = sizeof(vertices[0]);

	//インデックスデータ
	unsigned short indices[] =
	{
		0,1,2,	//三角形１つ目
		1,2,3,	//三角形２つ目
	};

	////インデックスデータ(線のやつ)
	//uint16_t indices[] =
	//{
	//	0,1,
	//	2,1,
	//	2,3,
	//	0,3,
	//	2,5,
	//	3,4,
	//	4,5
	//};

	//インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

	// リソース設定
	/*D3D12_RESOURCE_DESC resDesc{};*/
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB; // 頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//インデックスバッファの生成
	ID3D12Resource* indexBuff = nullptr;
	result = directX.device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);

	//インデックスバッファをマッピング
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//全インデックスに対して
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];	//インデックスをコピー
	}
	//マッピング解除
	indexBuff->Unmap(0, nullptr);

	// インデックスバッファビューの作成
	D3D12_INDEX_BUFFER_VIEW ibView{};
	// GPU仮想アドレス
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	//インデックス1個分のサイズ
	ibView.Format = DXGI_FORMAT_R16_UINT;
	// インデックスバッファのサイズ
	ibView.SizeInBytes = sizeIB;

	//テクスチャバッファ設定
	//ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//リソース設定
	D3D12_RESOURCE_DESC textureResouceDesc{};
	textureResouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResouceDesc.Format = metadate.format;
	textureResouceDesc.Width = metadate.width;	//幅
	textureResouceDesc.Height = (UINT)metadate.height;	//高さ
	textureResouceDesc.DepthOrArraySize = (UINT16)metadate.arraySize;
	textureResouceDesc.MipLevels = (UINT16)metadate.mipLevels;
	textureResouceDesc.SampleDesc.Count = 1;

	//テクスチャバッファの生成
	ID3D12Resource* texBuff = nullptr;

	result = directX.device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResouceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);

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

	//シェーダーリソースビューの作成
	//シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//設定構造体
	srvDesc.Format = resDesc.Format;	//RGBA float
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;

	//ハンドルの指す位置にシェーダーリソースビュー作成
	directX.device->CreateShaderResourceView(texBuff, &srvDesc, directX.srvHandle);

	ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
	ID3DBlob* errorBlob = nullptr; // エラーオブジェクト
	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicVS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);

	// エラーなら
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicPS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);

	// エラーなら
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	{
	"POSITION",										//セマンティック名
	0,												//同名のセマンティックがあるとき使うインデックス
	DXGI_FORMAT_R32G32B32_FLOAT,					//要素数とビット数を表す
	0,												//入力スロットインデックス
	D3D12_APPEND_ALIGNED_ELEMENT,					//データのオフセット地(左のは自動設定)
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		//入力データ種別
	0												//一度に描画するインスタンス数(0でよい)
	}, // (1行で書いたほうが見やすい)
	//座標以外に色、テクスチャUVなどを渡す場合はさらに続ける
		{
			"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		}
	};

	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	// シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	// ラスタライザの設定
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	// カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし(D3D12_FILL_MODE_WIREFRAMEにするとワイヤーフレームに)
	pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

	// ブレンドステート
	//pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
	//	= D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA全てのチャンネルを描画

	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//RBGA全てのチャンネルを描画

	blenddesc.BlendEnable = true;					//ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//デストの値を0%使う

	//加算合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;			//ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	//1.0f-ソースのアルファ値

	// 頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// その他の設定
	pipelineDesc.NumRenderTargets = 1; // 描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	//テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//横繰り返し（タイリング）
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//縦繰り返し（タイリング）
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//奥行繰り返し（タイリング）
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	//ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;					//全てリニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;									//ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;												//ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;			//ピクセルシェーダからのみ使用可能

	// ルートシグネチャ
	ID3D12RootSignature* rootSignature;
	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;					//ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);		//ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	// ルートシグネチャのシリアライズ
	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = directX.device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();
	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature;

	// パイプランステートの生成
	ID3D12PipelineState* pipelineState = nullptr;
	result = directX.device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));
#pragma endregion
	//ゲームループ
	while (true)
	{
#pragma region ウィンドウメッセージ処理
		if (PeekMessage(&win.msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&win.msg);	//キー入力メッセージの処理
			DispatchMessage(&win.msg);	//プロシージャにメッセージを送る
		}


		//アプリケーションが終わるときにmwssageがWM_QUITになる
		if (win.msg.message == WM_QUIT)
		{
			break;
		}
#pragma endregion

#pragma region DirectX毎フレーム処理
		//DirectX毎フレーム　ここから
		keyboard.InputUpdate();

		//値を書き込むと自動的に転送される
		constMapMaterial->color = XMFLOAT4(1, 0, 0, 0.5f);	//RGBAで半透明の赤

#pragma region グラフィックスコマンド
		// バックバッファの番号を取得(2つなので0番か1番)
		UINT bbIndex = directX.swapChain->GetCurrentBackBufferIndex();
		// 1.リソースバリアで書き込み可能に変更
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = directX.backBuffers[bbIndex]; // バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // 表示状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
		directX.commandList->ResourceBarrier(1, &barrierDesc);

		// 2.描画先の変更
		// レンダーターゲットビューのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = directX.rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * directX.device->GetDescriptorHandleIncrementSize(directX.rtvHeapDesc.Type);
		directX.commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		// 3.画面クリア R G B A
		FLOAT clearColor[] = { 0.1f,0.25f,0.5f,0.0f }; // 青っぽい色
		directX.commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		//if (key[DIK_SPACE])
		//{
		//	FLOAT clearColor[] = { 0.0f,1.0f, 0.0f,0.0f }; // 赤っぽい色
		//	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		//}

		// 4.描画コマンドここから
		// ビューポート設定コマンド
		D3D12_VIEWPORT viewport{};
		viewport.Width = win.width;
		viewport.Height = win.height;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;	//最小震度
		viewport.MaxDepth = 1.0f;	//最大深度
		// ビューポート設定コマンドを、コマンドリストに積む
		directX.commandList->RSSetViewports(1, &viewport);

		// シザー矩形
		D3D12_RECT scissorRect{};
		scissorRect.left = 0; // 切り抜き座標左
		scissorRect.right = scissorRect.left + win.width; // 切り抜き座標右
		scissorRect.top = 0; // 切り抜き座標上
		scissorRect.bottom = scissorRect.top + win.height; // 切り抜き座標下
		// シザー矩形設定コマンドを、コマンドリストに積む
		directX.commandList->RSSetScissorRects(1, &scissorRect);

		// パイプラインステートとルートシグネチャの設定コマンド
		directX.commandList->SetPipelineState(pipelineState);
		directX.commandList->SetGraphicsRootSignature(rootSignature);

		// プリミティブ形状の設定コマンド
		directX.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形リスト

		// 頂点バッファビューの設定コマンド
		directX.commandList->IASetVertexBuffers(0, 1, &vbView);

		//インデックスバッファビューの設定コマンド
		directX.commandList->IASetIndexBuffer(&ibView);

		//定数バッファビュー(CBV)の設定コマンド
		directX.commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
		//シェーダリソースビュー(SRV)ヒープの設定コマンド
		directX.commandList->SetDescriptorHeaps(1, &directX.srvHeap);
		//シェーダリソースビュー(SRV)ヒープの先頭ハンドルを取得(SRVを指してるはず)
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = directX.srvHeap->GetGPUDescriptorHandleForHeapStart();
		//シェーダリソースビュー(SRV)ヒープの先頭にあるSRVをルートパラメータ1番に設定
		directX.commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
		//定数バッファビュー（CRV）の設定コマンド
		//ルートパラメータ2番に3D変換行列の定数バッファを渡す
		directX.commandList->SetGraphicsRootConstantBufferView(2, directX.constBuffTransform->GetGPUVirtualAddress());

		// 描画コマンド
		directX.commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0); // インデックスバッファを使って描画
		// 4.描画コマンドここまで

		// 5.リソースバリアを戻す
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // 表示状態へ
		directX.commandList->ResourceBarrier(1, &barrierDesc);
#pragma endregion

		// 命令のクローズ
		result = directX.commandList->Close();
		assert(SUCCEEDED(result));
		// コマンドリストの実行
		ID3D12CommandList* commandLists[] = { directX.commandList };
		directX.commandQueue->ExecuteCommandLists(1, commandLists);
#pragma endregion

		// 画面に表示するバッファをフリップ(裏表の入替え)
		result = directX.swapChain->Present(1, 0);
		assert(SUCCEEDED(result));

		// コマンドの実行完了を待つ
		directX.commandQueue->Signal(directX.fence, ++directX.fenceVal);
		if (directX.fence->GetCompletedValue() != directX.fenceVal) {
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			directX.fence->SetEventOnCompletion(directX.fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
		// キューをクリア
		result = directX.commandAllocator->Reset();
		assert(SUCCEEDED(result));
		// 再びコマンドリストを貯める準備
		result = directX.commandList->Reset(directX.commandAllocator, nullptr);
		assert(SUCCEEDED(result));

		//DirectX毎フレーム　ここまで
	}
#pragma region WindowsAPI後始末
	//もうクラスは使わないので登録解除する
	UnregisterClass(win.w.lpszClassName, win.w.hInstance);
#pragma endregion

	return 0;
}