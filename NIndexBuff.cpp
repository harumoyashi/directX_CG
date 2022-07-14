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
    // ���_�f�[�^
    vertices.push_back({ -0.5f, -0.5f, 0.0f });
    vertices.push_back({ -0.5f, +0.5f, 0.0f });
    vertices.push_back({ +0.5f, -0.5f, 0.0f });

    // ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
    sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * vertices.size());
}

void IndexBuff::SetVerBuff()
{
    heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p

    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resDesc.Width = sizeVB; // ���_�f�[�^�S�̂̃T�C�Y
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
        &heapProp, // �q�[�v�ݒ�
        D3D12_HEAP_FLAG_NONE,
        &resDesc, // ���\�[�X�ݒ�
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertBuff));
    assert(SUCCEEDED(result));
}

void IndexBuff::TransferVerBuff()
{
    // GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
    HRESULT result;
    result = vertBuff->Map(0, nullptr, (void**)&vertMap);
    assert(SUCCEEDED(result));
    // �S���_�ɑ΂���
    for (int i = 0; i < vertices.size(); i++) {
        vertMap[i] = vertices[i]; // ���W���R�s�[
    }
    // �q���������
    vertBuff->Unmap(0, nullptr);
}

void IndexBuff::CreateVerBuffView()
{
    // GPU���z�A�h���X
    vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
    // ���_�o�b�t�@�̃T�C�Y
    vbView.SizeInBytes = sizeVB;
    // ���_1���̃f�[�^�T�C�Y
    vbView.StrideInBytes = sizeof(XMFLOAT3);
}
