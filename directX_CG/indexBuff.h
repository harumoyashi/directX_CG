#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <d3d12.h>
using namespace DirectX;
class IndexBuff
{
public:
	// ���_�f�[�^
	std::vector<XMFLOAT3>vertices;
	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB;

	D3D12_HEAP_PROPERTIES heapProp{}; // �q�[�v�ݒ�
	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	// ���_�o�b�t�@
	ID3D12Resource* vertBuff = nullptr;
	//���_�f�[�^�R�s�[�p
	XMFLOAT3* vertMap = nullptr;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};

public:
	void Initialize(ID3D12Device* device);
	void Update();

private:
	//���_�f�[�^�ݒ�
	void SetVertices();
	//���_�o�b�t�@�ݒ�
	void SetVerBuff();
	// ���_�o�b�t�@�̐���
	void CreateVerBuff(ID3D12Device* device);
	//���_�o�b�t�@�ւ̃f�[�^�]��
	void TransferVerBuff();
	// ���_�o�b�t�@�r���[�̍쐬
	void CreateVerBuffView();
};

