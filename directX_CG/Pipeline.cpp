#include "Pipeline.h"
void Pipeline::Initialize(ID3D12Device* device, int blendMode)
{
	//シェーダー
	VSCompile();
	PSCompile();

	//頂点レイアウト
	CreateVerLayout();

	// グラフィックスパイプライン設定
	SetShader();
	SetRasterizer();
	SetBlendState(blendMode);
	SetVerLayout();
	SetTopology();
}

void Pipeline::Update()
{
	//commandList->DrawInstanced(vertices.size(), 1, 0, 0); // 全ての頂点を使って描画
}

void Pipeline::VSCompile()
{
	HRESULT result;
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
}

void Pipeline::PSCompile()
{
	HRESULT result;
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
}

void Pipeline::CreateVerLayout()
{
	inputLayout.push_back(
		{
		"POSITION",										//セマンティック名
		0,												//同名のセマンティックがあるとき使うインデックス
		DXGI_FORMAT_R32G32B32_FLOAT,					//要素数とビット数を表す
		0,												//入力スロットインデックス
		D3D12_APPEND_ALIGNED_ELEMENT,					//データのオフセット地(左のは自動設定)
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		//入力データ種別
		0												//一度に描画するインスタンス数(0でよい)
		}// (1行で書いたほうが見やすい)
		//座標以外に色、テクスチャUVなどを渡す場合はさらに続ける
	);
}

void Pipeline::SetShader()
{
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
}

void Pipeline::SetRasterizer()
{
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	// カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	// ポリゴン内塗りつぶし(D3D12_FILL_MODE_WIREFRAMEにするとワイヤーフレームに)
	pipelineDesc.RasterizerState.DepthClipEnable = true;			// 深度クリッピングを有効に
}

void Pipeline::SetBlendState(int blendMode)
{
	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
		= D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA全てのチャンネルを描画

	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//RBGA全てのチャンネルを描画

	if (blendMode == noBlend)
	{
		blenddesc.BlendEnable = false;				//ブレンドを無効にする
	}
	else
	{
		blenddesc.BlendEnable = true;				//ブレンドを有効にする
	}
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//デストの値を0%使う

	//ブレンドモードの変更
	if (blendMode == add)
	{
		//加算合成
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;		//加算
		blenddesc.SrcBlend = D3D12_BLEND_ONE;		//ソースの値を100%使う
		blenddesc.DestBlend = D3D12_BLEND_ONE;		//デストの値を100%使う
	}
	else if (blendMode == sub)
	{
		//減算合成
		blenddesc.BlendOp = D3D12_BLEND_OP_SUBTRACT;	//減算
		blenddesc.SrcBlend = D3D12_BLEND_ONE;			//ソースのアルファ値
		blenddesc.DestBlend = D3D12_BLEND_ONE;			//1.0f-ソースのアルファ値
	}
	else if (blendMode == invers)
	{
		//色反転
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//加算
		blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;	//1.0f-ソースのアルファ値
		blenddesc.DestBlend = D3D12_BLEND_ZERO;				//使わない
	}
	else if (blendMode == alpha)
	{
		//半透明合成
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//加算
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;			//ソースのアルファ値
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	//1.0f-ソースのアルファ値
	}
}

void Pipeline::SetVerLayout()
{
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout.data();
	pipelineDesc.InputLayout.NumElements = inputLayout.size();
}

void Pipeline::SetTopology()
{
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// その他の設定
	pipelineDesc.NumRenderTargets = 1; // 描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング
}
