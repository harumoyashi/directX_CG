#pragma once
#include "NWindows.h"
#include <DirectXTex.h>

#include <d3d12.h>
#include <dxgi1_6.h>	//Visual Studio Graphics Debugger���듮��N��������A1_4�ɂ���Ɖ������邩��

#include <DirectXMath.h>
using namespace DirectX;

#include "NDirectXInput.h"

#include <wrl.h>
using namespace Microsoft::WRL;

class MyDirectX
{
public:
	HRESULT result;

	//�f�o�b�O���C���[���I���ɂ��邽�߂Ɏg�p�����C���^�[�t�F�C�X
	ComPtr<ID3D12Debug> debugController;

	//�萔�o�b�t�@�p�f�[�^�\���́i3D�ϊ��s��j
	struct  ConstBufferDataTransform
	{
		XMMATRIX mat;	//3D�ϊ��s��
	};

	//�萔�o�b�t�@0��
	ComPtr<ID3D12Resource> constBuffTransform0;
	ConstBufferDataTransform* constMapTransform0 = nullptr;
	//�萔�o�b�t�@1��
	ComPtr<ID3D12Resource> constBuffTransform1;
	ConstBufferDataTransform* constMapTransform1 = nullptr;

	//��{�I�u�W�F�N�g�̐���
	//�󂯎M�ɂȂ�ϐ�����

	//�F�X�Ȃ��̍쐬���邽�߂̉��z�A�_�v�^
	ComPtr<ID3D12Device> device;
	//�A�_�v�^�[�̗񋓏�Ԃ̕ω������o�ł���悤�ɂ��邽�߂̃C���^�[�t�F�[�X
	ComPtr<IDXGIFactory7> dxgiFactory;
	//�_�u���o�b�t�@�����O�̂��߂ɉ�ʐ؂�ւ��p�̃o�b�t�@�[�Ǘ�������
	ComPtr<IDXGISwapChain4> swapChain;
	//�R�}���h���X�g�Ɋi�[���閽�߂ׂ̈̃��������Ǘ�����I�u�W�F�N�g
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	//���߂��ꎞ�I�Ɋi�[���Ƃ����
	ComPtr<ID3D12GraphicsCommandList> commandList;
	//GPU�����s����ׂ����߂̃��X�g��,�w�肵��������GPU�ɓ]�����邽�߂̃C���^�[�t�F�[�X
	ComPtr<ID3D12CommandQueue> commandQueue;

	ComPtr<ID3D12DescriptorHeap> rtvHeap;

	ConstBufferDataTransform* constMapTransform = nullptr;
	//�萔�o�b�t�@��GPU���\�[�X�̃|�C���^
	ComPtr<ID3D12Resource> constBuffTransform;

	//�A�_�v�^�[�̗񋓗p
	std::vector<ComPtr<IDXGIAdapter4>> adapters;

	//�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	ComPtr<IDXGIAdapter4> tmpAdapter;

	D3D_FEATURE_LEVEL featureLevel;

	//�R�}���h�L���[�̐ݒ�
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};

	ID3D12DescriptorHeap* srvHeap = nullptr;

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};

	std::vector<ComPtr<ID3D12Resource>> backBuffers;

	//�V�F�[�_�[���\�[�X�r���[�n���h��
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;
	//�����_�[�^�[�Q�b�g�r���[�n���h��
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	//�����_�[�^�[�Q�b�g�r���[�̐ݒ�
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	//DirectX������
	void Init(HWND hwnd);

private:
	//�E�B���h�E�N���X
	NWindows win;

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

