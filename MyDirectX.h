#pragma once
#include "MyWindows.h"
#include <DirectXTex.h>

#include <d3d12.h>
#include <dxgi1_6.h>	//Visual Studio Graphics Debugger���듮��N��������A1_4�ɂ���Ɖ������邩��
#include <cassert>

#include <DirectXMath.h>
using namespace DirectX;

class MyDirectX
{
public:
	ID3D12Debug* debugController;

	//�萔�o�b�t�@�p�f�[�^�\���́i3D�ϊ��s��j
	struct  ConstBufferDataTransform
	{
		XMMATRIX mat;	//3D�ϊ��s��
	};

	//��{�I�u�W�F�N�g�̐���
	//�󂯎M�ɂȂ�ϐ�����
	HRESULT result;
	ID3D12Device* device = nullptr;
	IDXGIFactory7* dxgiFactory = nullptr;
	IDXGISwapChain4* swapChain = nullptr;
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12DescriptorHeap* rtvHeap = nullptr;

	ConstBufferDataTransform* constMapTransform = nullptr;
	ID3D12Resource* constBuffTransform = nullptr;			//�萔�o�b�t�@��GPU���\�[�X�̃|�C���^

	// �A�_�v�^�[�̗񋓗p
	std::vector<IDXGIAdapter4*> adapters;

	// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	IDXGIAdapter4* tmpAdapter = nullptr;

	D3D_FEATURE_LEVEL featureLevel;

	//�R�}���h�L���[�̐ݒ�
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	ID3D12DescriptorHeap* srvHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};

	std::vector<ID3D12Resource*> backBuffers;

	//�V�F�[�_�[���\�[�X�r���[�n���h��
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;
	//�����_�[�^�[�Q�b�g�r���[�n���h��
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	//�����_�[�^�[�Q�b�g�r���[�̐ݒ�
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

	//DirectX������
	void Init(HWND hwnd);

private:
	Windows win;

	//�A�_�v�^�[�I��
	void ChoiceAdapters();
	//�f�o�C�X�̐���
	void CreateDevice();
	//�R�}���h�A���P�[�^�A���X�g�A�L���[�̐���
	void CreateCommandGroup();
	// �X���b�v�`�F�[���̐���
	void CreateSwapChain(HWND hwnd);
	//�V�F�[�_���\�[�X�r���[�̃f�X�N���v�^�q�[�v����
	void CreateSRVHeapDesc();
	//�����_�[�^�[�Q�b�g�r���[�̃f�X�N���v�^�q�[�v����
	void CreateRTVHeapDesc();
};
