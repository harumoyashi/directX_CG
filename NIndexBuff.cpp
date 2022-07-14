#include "NIndexBuff.h"
void IndexBuff::Initialize(ID3D12Device* device)
{
    SetVertices();
    SetVerBuff();
    CreateVerBuff(device);
    TransferVerBuff();
    CreateVerBuffView();
}
void IndexBuff::Update()
{
}
void IndexBuff::SetVertices()
{
    // 頂点データ
    vertices.push_back({ -0.5f, -0.5f, 0.0f });
    vertices.push_back({ -0.5f, +0.5f, 0.0f });
    vertices.push_back({ +0.5f, -0.5f, 0.0f });

    // 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
    sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * vertices.size());
}

void IndexBuff::SetVerBuff()
{
    heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用

    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resDesc.Width = sizeVB; // 頂点データ全体のサイズ
    resDesc.Height = 1;
    resDesc.DepthOrArraySize = 1;
    resDesc.MipLevels = 1;
    resDesc.SampleDesc.Count = 1;
    resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
}

void IndexBuff::CreateVerBuff(ID3D12Device* device)
{
    HRESULT result;
    result = device->CreateCommittedResource(
        &heapProp, // ヒープ設定
        D3D12_HEAP_FLAG_NONE,
        &resDesc, // リソース設定
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertBuff));
    assert(SUCCEEDED(result));
}

void IndexBuff::TransferVerBuff()
{
    // GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
    HRESULT result;
    result = vertBuff->Map(0, nullptr, (void**)&vertMap);
    assert(SUCCEEDED(result));
    // 全頂点に対して
    for (int i = 0; i < vertices.size(); i++) {
        vertMap[i] = vertices[i]; // 座標をコピー
    }
    // 繋がりを解除
    vertBuff->Unmap(0, nullptr);
}

void IndexBuff::CreateVerBuffView()
{
    // GPU仮想アドレス
    vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
    // 頂点バッファのサイズ
    vbView.SizeInBytes = sizeVB;
    // 頂点1つ分のデータサイズ
    vbView.StrideInBytes = sizeof(XMFLOAT3);
}
