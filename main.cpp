#include "MyWindows.h"
#include "MyDirectX.h"
#include "Vector3.h"
#include <vector>
#include "Matrix4.h"
#include "Texture.h"
#include <string>
#include <random>

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

//3Dオブジェクト型
struct Object3d
{
	//定数バッファ（行列用）
	ID3D12Resource* constBuffTransform;
	//定数バッファマップ（行列用）
	MyDirectX::ConstBufferDataTransform* constMapTransform;
	//アフィン変換情報
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 position = { 0,0,0 };
	//ワールド変換行列
	XMMATRIX matWorld;
	//親オブジェクトへのポインタ
	Object3d* parent = nullptr;

	void InitializeObject3d(Object3d* object, ID3D12Device* device)
	{
		HRESULT result;

		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPUへの転送用
		//リソース設定
		D3D12_RESOURCE_DESC cbResouceDesc{};
		cbResouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResouceDesc.Width = (sizeof(constMapTransform) + 0xff) & ~0xff;	//256バイトアラインメント
		cbResouceDesc.Height = 1;
		cbResouceDesc.DepthOrArraySize = 1;
		cbResouceDesc.MipLevels = 1;
		cbResouceDesc.SampleDesc.Count = 1;
		cbResouceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		//定数バッファの生成
		result = device->CreateCommittedResource(
			&cbHeapProp,	//ヒープ設定
			D3D12_HEAP_FLAG_NONE,
			&cbResouceDesc,	//リソース設定
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuffTransform)
		);
		assert(SUCCEEDED(result));

		//定数バッファのマッピング
		result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);	//マッピング
		assert(SUCCEEDED(result));
	}

	void UpdateObject3d(Object3d* object, XMMATRIX& matView, XMMATRIX& matProjection)
	{
		XMMATRIX matScale, matRot, matTrans;

		//スケール、回転、平行移動行列の計算
		matScale = XMMatrixScaling(object->scale.x, object->scale.y, object->scale.z);
		matRot = XMMatrixIdentity();
		matRot *= XMMatrixRotationZ(object->rotation.z);
		matRot *= XMMatrixRotationX(object->rotation.x);
		matRot *= XMMatrixRotationY(object->rotation.y);
		matTrans = XMMatrixTranslation(object->position.x, object->position.y, object->position.z);

		//ワールド行列を合成
		object->matWorld = XMMatrixIdentity();
		object->matWorld = matTrans * matRot * matTrans;

		if (object->parent != nullptr)
		{
			//親オブジェクトのワールド行列をかける
			object->matWorld *= object->parent->matWorld;
		}

		//定数バッファデータ転送
		object->constMapTransform->mat = object->matWorld * matView * matProjection;
	}

	void DrawObject3d(Object3d* object, ID3D12GraphicsCommandList* commandList,
		D3D12_VERTEX_BUFFER_VIEW& vbView, D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices)
	{
		//頂点バッファの設定
		commandList->IASetVertexBuffers(0, 1, &vbView);
		//インデックスバッファの設定
		commandList->IASetIndexBuffer(&ibView);
		//定数バッファビュー(CBV)の設定コマンド
		commandList->SetGraphicsRootConstantBufferView(2, object->constBuffTransform->GetGPUVirtualAddress());

		//描画コマンド
		commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
	}
};
#pragma endregion

enum PartId
{
	kRoot,  //大元
	kSpine, //脊椎
	kChest, //胸
	kHead,  //頭
	kArmL,  //左腕
	kArmR,  //右腕
	kHip,   //おしり
	kLegL,  //左足
	kLegR,  //右足

	kNumPartId
};

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
	DirectXInput key;
	key.InputInit(win.w, win.hwnd);
	//DirectX初期化ここまで
#pragma endregion
#pragma region 描画初期化処理
	//深度バッファ
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = win.width;	//レンダーターゲットに合わせる
	depthResourceDesc.Height = win.height;	//レンダーターゲットに合わせる
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	//デプスステンシル

	//深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;	//深度値1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット

	//リソース生成
	ID3D12Resource* depthBuff = nullptr;
	result = directX.device->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//深度値書き込みに使用
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff)
	);

	//深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;	//深度ビューは1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;	//デプスステンシルビュー
	ID3D12DescriptorHeap* dsvHeap = nullptr;
	result = directX.device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	//深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	directX.device->CreateDepthStencilView(
		depthBuff,
		&dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

	//CPUとGPUの同期に使われるやつ
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

	// フェンスの生成
	result = directX.device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(result));

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
		////ヒープ設定
		//D3D12_HEAP_PROPERTIES cbHeapProp{};
		//cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPUへの転送用
		////リソース設定
		//D3D12_RESOURCE_DESC cbResouceDesc{};
		//cbResouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		//cbResouceDesc.Width = (sizeof(directX.constMapTransform) + 0xff) & ~0xff;	//256バイトアラインメント
		//cbResouceDesc.Height = 1;
		//cbResouceDesc.DepthOrArraySize = 1;
		//cbResouceDesc.MipLevels = 1;
		//cbResouceDesc.SampleDesc.Count = 1;
		//cbResouceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		////定数バッファ0番の生成
		//result = directX.device->CreateCommittedResource(
		//	&cbHeapProp,	//ヒープ設定
		//	D3D12_HEAP_FLAG_NONE,
		//	&cbResouceDesc,	//リソース設定
		//	D3D12_RESOURCE_STATE_GENERIC_READ,
		//	nullptr,
		//	IID_PPV_ARGS(&directX.constBuffTransform0)
		//);
		//assert(SUCCEEDED(result));

		////定数バッファ0番のマッピング
		//result = directX.constBuffTransform0->Map(0, nullptr, (void**)&directX.constMapTransform0);	//マッピング
		//assert(SUCCEEDED(result));

		////定数バッファ1番の生成
		//result = directX.device->CreateCommittedResource(
		//	&cbHeapProp,	//ヒープ設定
		//	D3D12_HEAP_FLAG_NONE,
		//	&cbResouceDesc,	//リソース設定
		//	D3D12_RESOURCE_STATE_GENERIC_READ,
		//	nullptr,
		//	IID_PPV_ARGS(&directX.constBuffTransform1)
		//);
		//assert(SUCCEEDED(result));

		////定数バッファ1番のマッピング
		//result = directX.constBuffTransform1->Map(0, nullptr, (void**)&directX.constMapTransform1);	//マッピング
		//assert(SUCCEEDED(result));

	}
	//3Dオブジェクトの数
	const size_t kObjectCount = kNumPartId;
	//3Dオブジェクトの配列
	Object3d object3ds[kObjectCount];

	////乱数シード生成器
	//std::random_device seed_gen;
	////メルセンヌ・ツイスター
	//std::mt19937_64 engine(seed_gen());

	for (int i = 0; i < _countof(object3ds); i++)
	{
		////乱数範囲(回転角用)
		//std::uniform_real_distribution<float> rotDist(0.0f, XMConvertToDegrees(180.0f));
		////乱数範囲(座標用)
		//std::uniform_real_distribution<float> posDist(-30.0f, 30.0f);

		//初期化
		object3ds[i].InitializeObject3d(&object3ds[i], directX.device);

		//親子構造のサンプル↓
		if (i > 0)
		{
			//ひとつ前のオブジェクトを親オブジェクトとする
			/*object3ds[i].parent = &object3ds[i - 1];*/
			//親オブジェクトの9割の大きさ
			object3ds[i].scale = { 0.9f,0.9f,0.9f };
			//回転角設定
			object3ds[i].rotation = { 0,0,0 };

			////親オブジェクトに対してZ方向-8.0ずらす
			//object3ds[i].position = { 0,0,0 };
		}
	}

	object3ds[PartId::kRoot].position = { 0,0,0 };

	object3ds[PartId::kSpine].position = { 0,8.0f,0 };
	object3ds[PartId::kSpine].parent = &object3ds[PartId::kRoot];

	object3ds[PartId::kChest].position = { 0,0,0 };
	object3ds[PartId::kChest].parent = &object3ds[PartId::kSpine];

	object3ds[PartId::kHead].position = { 0,8.0f,0 };
	object3ds[PartId::kHead].parent = &object3ds[PartId::kChest];

	object3ds[PartId::kArmL].position = { -8.0f,0,0 };
	object3ds[PartId::kArmL].parent = &object3ds[PartId::kChest];

	object3ds[PartId::kArmR].position = { 8.0f,0,0 };
	object3ds[PartId::kArmR].parent = &object3ds[PartId::kChest];

	object3ds[PartId::kHip].position = { 0,-8.0f,0 };
	object3ds[PartId::kHip].parent = &object3ds[PartId::kSpine];

	object3ds[PartId::kLegL].position = { -8.0f,-8.0f,0 };
	object3ds[PartId::kLegL].parent = &object3ds[PartId::kHip];

	object3ds[PartId::kLegR].position = { 8.0f,-8.0f,0 };
	object3ds[PartId::kLegR].parent = &object3ds[PartId::kHip];

#pragma region 行列の計算
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

	////平行投影変換//
	////単位行列を代入
	//directX.constMapTransform0->mat = XMMatrixIdentity();

	///*constMapTransform->mat.r[0].m128_f32[0] = 2.0f / window_width;
	//constMapTransform->mat.r[1].m128_f32[1] = -2.0f / window_height;

	//constMapTransform->mat.r[3].m128_f32[0] = -1.0f;
	//constMapTransform->mat.r[3].m128_f32[1] = 1.0f;*/

	////平行投影変換
	//directX.constMapTransform0->mat = XMMatrixOrthographicOffCenterLH(
	//	0, win.width,
	//	win.height, 0,
	//	0.0f, 1.0f
	//);

	////透視投影変換//
	//directX.constMapTransform0->mat = XMMatrixPerspectiveFovLH(
	//	XMConvertToRadians(45.0f),		//上下画角45度
	//	(float)win.width / win.height,	//アスペクト比(画面横幅/画面縦幅)
	//	0.1f, 1000.0f					//前端、奥端
	//);

	//射影投影変換//
	XMMATRIX matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),		//上下画角45度
		(float)win.width / win.height,	//アスペクト比(画面横幅/画面縦幅)
		0.1f, 1000.0f					//前端、奥端
	);

	//ここでビュー変換行列計算
	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -100);	//視点座標
	XMFLOAT3 target(0, 0, 0);	//注視点座標
	XMFLOAT3 up(0, 1, 0);		//上方向ベクトル
	//ビュー変換行列作成
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	float angle = 0.0f;	//カメラの回転角

	//XMFLOAT3 scale = { 1.0f,1.0f,1.0f };	//スケーリング倍率
	//XMFLOAT3 rotation = { 0.0f,0.0f,0.0f };	//回転角
	//XMFLOAT3 position = { 0.0f,0.0f,0.0f };	//座標
#pragma endregion

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
	 
	//1枚目の画像
	Texture texture[maxTexture];
	texture[0].Load(L"Resources/mario.jpg");	
	//2枚目の画像
	texture[1].Load(L"Resources/reimu.png");

	for (size_t i = 0; i < maxTexture; i++)
	{
		texture[i].CreateMipmap();
	}

	//頂点データ構造体
	struct Vertex
	{
		XMFLOAT3 pos;		//xyz座標
		XMFLOAT3 normal;	//法線ベクトル
		XMFLOAT2 uv;		//uv座標
	};

	// 頂点データ
	Vertex vertices[] = {
		//	x		y		z	 法線	u	v
		//前
		{{ -5.0f, -5.0f, -5.0f }, {}, {0.0f,1.0f}},	// 左下
		{{ -5.0f,  5.0f, -5.0f }, {}, {0.0f,0.0f}},	// 左上
		{{  5.0f, -5.0f, -5.0f }, {}, {1.0f,1.0f}},	// 右下
		{{  5.0f,  5.0f, -5.0f }, {}, {1.0f,0.0f}},	// 右上

		//後
		{{ -5.0f, -5.0f, 5.0f }, {}, {0.0f,1.0f}},	// 左下
		{{ -5.0f,  5.0f, 5.0f }, {}, {0.0f,0.0f}},	// 左上
		{{  5.0f, -5.0f, 5.0f }, {}, {1.0f,1.0f}},	// 右下
		{{  5.0f,  5.0f, 5.0f }, {}, {1.0f,0.0f}},	// 右上

		 // 左
		{{-5.0f,-5.0f,-5.0f }, {}, {0.0f, 1.0f}},    // 左下
		{{-5.0f,-5.0f, 5.0f }, {}, {0.0f, 0.0f}},    // 左上
		{{-5.0f, 5.0f,-5.0f }, {}, {1.0f, 1.0f}},    // 右下
		{{-5.0f, 5.0f, 5.0f }, {}, {1.0f, 0.0f}},    // 右上

		// 右
		{{ 5.0f,-5.0f,-5.0f }, {}, {0.0f, 1.0f}},    // 左下
		{{ 5.0f,-5.0f, 5.0f }, {}, {0.0f, 0.0f}},    // 左上
		{{ 5.0f, 5.0f,-5.0f }, {}, {1.0f, 1.0f}},    // 右下
		{{ 5.0f, 5.0f, 5.0f }, {}, {1.0f, 0.0f}},    // 右上

		// 上
		{{-5.0f,-5.0f,-5.0f }, {}, {0.0f, 1.0f}},    // 左下
		{{ 5.0f,-5.0f,-5.0f }, {}, {0.0f, 0.0f}},    // 左上
		{{-5.0f,-5.0f, 5.0f }, {}, {1.0f, 1.0f}},    // 右下
		{{ 5.0f,-5.0f, 5.0f }, {}, {1.0f, 0.0f}},    // 右上

		// 下
		{{-5.0f, 5.0f,-5.0f }, {}, {0.0f, 1.0f}},    // 左下
		{{ 5.0f, 5.0f,-5.0f }, {}, {0.0f, 0.0f}},    // 左上
		{{-5.0f, 5.0f, 5.0f }, {}, {1.0f, 1.0f}},    // 右下
		{{ 5.0f, 5.0f, 5.0f }, {}, {1.0f, 0.0f}},    // 右上
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

	//インデックスデータ
	unsigned short indices[] =
	{
		//前
		0,1,2,	//三角形1つ目
		2,1,3,	//三角形2つ目
		//後
		5,4,6,	//三角形3つ目
		5,6,7,	//三角形4つ目
		//左
		8,9,10,	//三角形5つ目
		10,9,11,//三角形6つ目
		//右
		13,12,14,	//三角形7つ目
		13,14,15,	//三角形8つ目
		//下
		16,17,18,	//三角形9つ目
		18,17,19,	//三角形10つ目
		//上
		21,20,22,	//三角形11つ目
		21,22,23,	//三角形12つ目
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

	for (int i = 0; i < _countof(indices) / 3; i++)
	{//三角形1つごとに計算していく
		//三角形のインデックスを取り出して、一時的な変数にいれる
		unsigned short index0 = indices[i * 3 + 0];
		unsigned short index1 = indices[i * 3 + 1];
		unsigned short index2 = indices[i * 3 + 2];

		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);

		//p0→p1ベクトル、p0→p2ベクトルを計算(ベクトルの減算)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		//外積は両方から垂直なベクトル
		XMVECTOR normal = XMVector3Cross(v1, v2);

		//正規化
		normal = XMVector3Normalize(normal);

		//求めた法線を頂点データに代入
		XMStoreFloat3(&vertices[index0].normal, normal);
		XMStoreFloat3(&vertices[index1].normal, normal);
		XMStoreFloat3(&vertices[index2].normal, normal);
	}

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

	for (size_t i = 0; i < maxTexture; i++)
	{
		texture[i].SetResouce();
		texture[i].CreateTexBuff(directX.device, textureHeapProp);
		texture[i].MipmapDataSend();
	}

	//シェーダーリソースビューの作成
	texture[0].CreateSRV(directX.device, directX.srvHandle);

	//1つハンドルを進める
	UINT incrementSize = directX.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	directX.srvHandle.ptr += incrementSize;

	//シェーダーリソースビュー設定(2個目)
	texture[1].CreateSRV(directX.device, directX.srvHandle);

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
		//xyz座標//
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
		//法線ベクトル//
			{
				"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
			},
		//uv座標//
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
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;	// 背面をカリング
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

	//デプスステンシルステートの設定
	pipelineDesc.DepthStencilState.DepthEnable = true;
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	//書き込み許可
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;	//小さければ合格
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット

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
		key.InputUpdate();

#pragma region 行列の計算
		if (key.IsKeyDown(DIK_D) || key.IsKeyDown(DIK_A))
		{
			if (key.IsKeyDown(DIK_D)) angle += XMConvertToRadians(10.0f);
			else if (key.IsKeyDown(DIK_A)) angle -= XMConvertToRadians(10.0f);

			//angleラジアンだけY軸周りに回転。半径は-100
			eye.x = -100 * sinf(angle);
			eye.z = -100 * cosf(angle);
			//ビュー変換行列再作成
			matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
		}

		//座標操作
		if (key.IsKeyDown(DIK_UP) || key.IsKeyDown(DIK_DOWN) || key.IsKeyDown(DIK_RIGHT) || key.IsKeyDown(DIK_LEFT))
		{
			if (key.IsKeyDown(DIK_UP)) { object3ds[0].position.y += 1.0f; }
			else if (key.IsKeyDown(DIK_DOWN)) { object3ds[0].position.y -= 1.0f; }
			if (key.IsKeyDown(DIK_RIGHT)) { object3ds[0].position.x += 1.0f; }
			else if (key.IsKeyDown(DIK_LEFT)) { object3ds[0].position.x -= 1.0f; }
		}

		//上半身回転
		if (key.IsKeyDown(DIK_U) || key.IsKeyDown(DIK_I))
		{
			if (key.IsKeyDown(DIK_U)) { object3ds[kChest].rotation.y += 0.05f; }
			else if (key.IsKeyDown(DIK_I)) { object3ds[kChest].rotation.y -= 0.05f; }
		}

		//下半身回転
		if (key.IsKeyDown(DIK_J) || key.IsKeyDown(DIK_K))
		{
			if (key.IsKeyDown(DIK_J)) { object3ds[kHip].rotation.y += 0.05f; }
			else if (key.IsKeyDown(DIK_K)) { object3ds[kHip].rotation.y -= 0.05f; }
		}

		/*eye.z -= 1.0f;*/
		////ビュー変換行列再作成
		//matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

		//rotation.x += 0.2f;
		//rotation.y += 0.2f;

		////いずれかのキーを押したとき
		//if (key.IsKeyDown(DIK_W) || key.IsKeyDown(DIK_S) || key.IsKeyDown(DIK_D) || key.IsKeyDown(DIK_A))
		//{
		//	if (key.IsKeyDown(DIK_W)) { position.y += 2.0f; }
		//	else if (key.IsKeyDown(DIK_S)) { position.y -= 2.0f; }
		//	if (key.IsKeyDown(DIK_D)) { position.x += 2.0f; }
		//	else if (key.IsKeyDown(DIK_A)) { position.x -= 2.0f; }
		//}

		for (size_t i = 0; i < _countof(object3ds); i++)
		{
			object3ds[i].UpdateObject3d(&object3ds[i], matView, matProjection);
		}

		////ワールド行列
		//XMMATRIX matScale;	//スケーリング行列
		//matScale = XMMatrixScaling(scale.x, scale.y, scale.z);

		//XMMATRIX matRot = XMMatrixIdentity();	//回転行列
		//matRot *= XMMatrixRotationZ(rotation.z);	//Z軸周りに0度回転してから
		//matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));	//X軸周りに15度回転してから
		//matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));	//Y軸周りに30度回転

		//XMMATRIX matTrans;	//平行移動行列
		//matTrans = XMMatrixTranslation(position.x, position.y, position.z);

		//XMMATRIX matWorld = XMMatrixIdentity();	//単位行列代入
		//matWorld *= matScale;	//ワールド座標にスケーリングを反映
		//matWorld *= matRot;	//ワールド座標に回転を反映
		//matWorld *= matTrans;	//ワールド座標に平行移動を反映

		////定数バッファに送信
		//directX.constMapTransform0->mat = matWorld * matView * matProjection;
#pragma endregion
		////ワールド変換行列
		//XMMATRIX matWorld1 = XMMatrixIdentity();

		////各種変形行列を計算
		//XMMATRIX matScale1 = XMMatrixScaling(1.0f, 1.0f, 1.0f);
		//XMMATRIX matRot1 = XMMatrixRotationY(XM_PI / 4.0f);
		//XMMATRIX matTrans1 = XMMatrixTranslation(-20.0f, 0, 0);

		////ワールド行列を合成
		//matWorld1 = matTrans1 * matRot1 * matTrans1;

		////ワールド、ビュー、射影行列を合成してシェーダーに転送
		//directX.constMapTransform1->mat = matWorld1 * matView * matProjection;

		//値を書き込むと自動的に転送される
		constMapMaterial->color = XMFLOAT4(1, 1, 1, 1);	//RGBAで半透明の赤

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
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
		directX.commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

		// 3.画面クリア R G B A
		FLOAT clearColor[] = { 0.1f,0.25f,0.5f,0.0f }; // 青っぽい色
		directX.commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		directX.commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

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
		if (key.IsKeyDown(DIK_SPACE))
		{
			srvGpuHandle.ptr += incrementSize;
		}
		//シェーダリソースビュー(SRV)ヒープの先頭にあるSRVをルートパラメータ1番に設定
		directX.commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
		////0番定数バッファビュー（CRV）の設定コマンド
		////ルートパラメータ2番に3D変換行列の定数バッファを渡す
		//directX.commandList->SetGraphicsRootConstantBufferView(2, directX.constBuffTransform0->GetGPUVirtualAddress());
		//// 描画コマンド
		//directX.commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0); // インデックスバッファを使って描画

		////1番定数バッファビュー（CRV）の設定コマンド
		////ルートパラメータ2番に3D変換行列の定数バッファを渡す
		//directX.commandList->SetGraphicsRootConstantBufferView(2, directX.constBuffTransform1->GetGPUVirtualAddress());
		//// 描画コマンド
		//directX.commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0); // インデックスバッファを使って描画

		for (int i = kChest; i < _countof(object3ds); i++)
		{
			object3ds[i].DrawObject3d(&object3ds[i], directX.commandList, vbView, ibView, _countof(indices));
		}
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
		directX.commandQueue->Signal(fence, ++fenceVal);
		if (fence->GetCompletedValue() != fenceVal) {
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceVal, event);
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