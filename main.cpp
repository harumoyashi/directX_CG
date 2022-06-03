#include "MyWindows.h"
#include "MyDirectX.h"
#include "Vector3.h"
#include <vector>
#include "Matrix4.h"
#include <string>
#include <random>

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include "DirectXInput.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#pragma region �\���̐錾
//�萔�o�b�t�@�p�\����
struct ConstBufferDataMaterial
{
	XMFLOAT4 color;	//�F(RGBA)
};

//3D�I�u�W�F�N�g�^
struct Object3d
{
	//�萔�o�b�t�@�i�s��p�j
	ID3D12Resource* constBuffTransform;
	//�萔�o�b�t�@�}�b�v�i�s��p�j
	MyDirectX::ConstBufferDataTransform* constMapTransform;
	//�A�t�B���ϊ����
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 position = { 0,0,0 };
	//���[���h�ϊ��s��
	XMMATRIX matWorld;
	//�e�I�u�W�F�N�g�ւ̃|�C���^
	Object3d* parent = nullptr;

	void InitializeObject3d(Object3d* object, ID3D12Device* device)
	{
		HRESULT result;

		//�q�[�v�ݒ�
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPU�ւ̓]���p
		//���\�[�X�ݒ�
		D3D12_RESOURCE_DESC cbResouceDesc{};
		cbResouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResouceDesc.Width = (sizeof(constMapTransform) + 0xff) & ~0xff;	//256�o�C�g�A���C�������g
		cbResouceDesc.Height = 1;
		cbResouceDesc.DepthOrArraySize = 1;
		cbResouceDesc.MipLevels = 1;
		cbResouceDesc.SampleDesc.Count = 1;
		cbResouceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		//�萔�o�b�t�@�̐���
		result = device->CreateCommittedResource(
			&cbHeapProp,	//�q�[�v�ݒ�
			D3D12_HEAP_FLAG_NONE,
			&cbResouceDesc,	//���\�[�X�ݒ�
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuffTransform)
		);
		assert(SUCCEEDED(result));

		//�萔�o�b�t�@�̃}�b�s���O
		result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);	//�}�b�s���O
		assert(SUCCEEDED(result));
	}

	void UpdateObject3d(Object3d* object, XMMATRIX& matView, XMMATRIX& matProjection)
	{
		XMMATRIX matScale, matRot, matTrans;

		//�X�P�[���A��]�A���s�ړ��s��̌v�Z
		matScale = XMMatrixScaling(object->scale.x, object->scale.y, object->scale.z);
		matRot = XMMatrixIdentity();
		matRot *= XMMatrixRotationZ(object->rotation.z);
		matRot *= XMMatrixRotationX(object->rotation.x);
		matRot *= XMMatrixRotationY(object->rotation.y);
		matTrans = XMMatrixTranslation(object->position.x, object->position.y, object->position.z);

		//���[���h�s�������
		object->matWorld = XMMatrixIdentity();
		object->matWorld = matScale * matRot * matTrans;

		if (object->parent != nullptr)
		{
			//�e�I�u�W�F�N�g�̃��[���h�s���������
			object->matWorld *= object->parent->matWorld;
		}

		//�萔�o�b�t�@�f�[�^�]��
		object->constMapTransform->mat = object->matWorld * matView * matProjection;
	}

	void DrawObject3d(Object3d* object, ID3D12GraphicsCommandList* commandList,
		D3D12_VERTEX_BUFFER_VIEW& vbView, D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices)
	{
		//���_�o�b�t�@�̐ݒ�
		commandList->IASetVertexBuffers(0, 1, &vbView);
		//�C���f�b�N�X�o�b�t�@�̐ݒ�
		commandList->IASetIndexBuffer(&ibView);
		//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
		commandList->SetGraphicsRootConstantBufferView(2, object->constBuffTransform->GetGPUVirtualAddress());

		//�`��R�}���h
		commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
	}
};
#pragma endregion

//�e�q�\���ԍ����蓖��
enum PartId
{
	kRoot,		//�匳
	kSpine,		//�Ғ�
	kChest,		//��
	kHead,		//��
	kUpArmL,	//����̘r
	kForeArmL,	//���O�r
	kHandL,		//����
	kUpArmR,	//�E��̘r
	kForeArmR,	//�E�O�r
	kHandR,		//�E��
	kWaist,		//��
	kHip,		//������
	kLegRootL,	//���r�t����
	kLegL,		//���r
	kFootL,		//����
	kLegRootR,	//�E�r�t����
	kLegR,		//�E�r
	kFootR,		//�E��

	kNumPartId	//�\���̂̐�
};

float armRotSpeed;
float fArmRotSpeed;

//Windows�A�v���ł̃G���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#pragma region WindowsAPI������
	Windows win;	//�E�B���h�E�N���X
	win.Set();
	win.CreateWindowObj();
	win.Display();
#pragma endregion
#pragma region DirectX������
	//DirectX��������������
	HRESULT result;
	MyDirectX directX;
	directX.Init(win.hwnd);

	//input������
	DirectXInput key;
	key.InputInit(win.w, win.hwnd);
	//DirectX�����������܂�
#pragma endregion
#pragma region �`�揉��������
	//�[�x�o�b�t�@
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = win.width;	//�����_�[�^�[�Q�b�g�ɍ��킹��
	depthResourceDesc.Height = win.height;	//�����_�[�^�[�Q�b�g�ɍ��킹��
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;	//�[�x�l�t�H�[�}�b�g
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	//�f�v�X�X�e���V��

	//�[�x�l�p�q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//�[�x�l�̃N���A�ݒ�
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;	//�[�x�l1.0f(�ő�l)�ŃN���A
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;	//�[�x�l�t�H�[�}�b�g

	//���\�[�X����
	ID3D12Resource* depthBuff = nullptr;
	result = directX.device->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//�[�x�l�������݂Ɏg�p
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff)
	);

	//�[�x�r���[�p�f�X�N���v�^�q�[�v�쐬
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;	//�[�x�r���[��1��
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;	//�f�v�X�X�e���V���r���[
	ID3D12DescriptorHeap* dsvHeap = nullptr;
	result = directX.device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	//�[�x�r���[�쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;	//�[�x�l�t�H�[�}�b�g
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	directX.device->CreateDepthStencilView(
		depthBuff,
		&dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

	//CPU��GPU�̓����Ɏg������
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

	// �t�F���X�̐���
	result = directX.device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(result));

	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResouceDesc{};
	cbResouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResouceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;	//256�o�C�g�A���C�������g
	cbResouceDesc.Height = 1;
	cbResouceDesc.DepthOrArraySize = 1;
	cbResouceDesc.MipLevels = 1;
	cbResouceDesc.SampleDesc.Count = 1;
	cbResouceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ID3D12Resource* constBuffMaterial = nullptr;
	//�萔�o�b�t�@�̐���
	result = directX.device->CreateCommittedResource(
		&cbHeapProp,	//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResouceDesc,	//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial)
	);
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�̃}�b�s���O
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);	//�}�b�s���O
	assert(SUCCEEDED(result));

	//3D�I�u�W�F�N�g�̐�
	const size_t kObjectCount = kNumPartId;
	//3D�I�u�W�F�N�g�̔z��
	Object3d object3ds[kObjectCount];

	//�I�u�W�F�N�g������
	for (int i = 0; i < _countof(object3ds); i++)
	{
		object3ds[i].InitializeObject3d(&object3ds[i], directX.device);
	}

	//�e�p�[�c������
	const float jointDistance = 15.0f;

	object3ds[PartId::kRoot].position = { 0,0,0 };

	object3ds[PartId::kSpine].position = { 0,jointDistance,0 };
	object3ds[PartId::kSpine].parent = &object3ds[PartId::kRoot];

	object3ds[PartId::kChest].position = { 0,jointDistance,0 };
	object3ds[PartId::kChest].parent = &object3ds[PartId::kWaist];

	object3ds[PartId::kHead].position = { 0,jointDistance,0 };
	object3ds[PartId::kHead].scale = { 1.5f,1.5f,1.5f };
	object3ds[PartId::kHead].parent = &object3ds[PartId::kChest];

	object3ds[PartId::kUpArmL].position = { -jointDistance * 0.8f,-2.0f,0 };
	object3ds[PartId::kUpArmL].rotation = { 0,0,-0.05f };
	object3ds[PartId::kUpArmL].scale = { 0.5f,1.3f,0.5f };
	object3ds[PartId::kUpArmL].parent = &object3ds[PartId::kChest];

	object3ds[PartId::kForeArmL].position = { 0,-jointDistance * 0.8f,0 };
	object3ds[PartId::kForeArmL].scale = { 1.0f,1.0f,1.0f };
	object3ds[PartId::kForeArmL].parent = &object3ds[PartId::kUpArmL];

	object3ds[PartId::kHandL].position = { 0,-jointDistance * 0.7f,0 };
	object3ds[PartId::kHandL].scale = { 1.0f,0.5f,1.0f };
	object3ds[PartId::kHandL].parent = &object3ds[PartId::kForeArmL];

	object3ds[PartId::kUpArmR].position = { jointDistance * 0.8f,-2.0f,0 };
	object3ds[PartId::kUpArmR].rotation = { 0,0,0.05f };
	object3ds[PartId::kUpArmR].scale = { 0.5f,1.3f,0.5f };
	object3ds[PartId::kUpArmR].parent = &object3ds[PartId::kChest];

	object3ds[PartId::kForeArmR].position = { 0,-jointDistance * 0.8f,0 };
	object3ds[PartId::kForeArmR].scale = { 1.0f,1.0f,1.0f };
	object3ds[PartId::kForeArmR].parent = &object3ds[PartId::kUpArmR];

	object3ds[PartId::kHandR].position = { 0,-jointDistance * 0.7f,0 };
	object3ds[PartId::kHandR].scale = { 1.0f,0.5f,1.0f };
	object3ds[PartId::kHandR].parent = &object3ds[PartId::kForeArmR];

	object3ds[PartId::kWaist].position = { 0,-jointDistance,0 };
	object3ds[PartId::kWaist].parent = &object3ds[PartId::kSpine];

	object3ds[PartId::kHip].position = { 0,-jointDistance,0 };
	object3ds[PartId::kHip].parent = &object3ds[PartId::kWaist];

	object3ds[PartId::kLegRootL].position = { -jointDistance * 0.5f,0,0 };
	object3ds[PartId::kLegRootL].parent = &object3ds[PartId::kHip];

	object3ds[PartId::kLegL].position = { 0,-jointDistance,0 };
	object3ds[PartId::kLegL].scale = { 0.8f,2.0f,0.8f };
	object3ds[PartId::kLegL].parent = &object3ds[PartId::kLegRootL];

	object3ds[PartId::kFootL].position = { 0,-jointDistance * 0.8f,0 };
	object3ds[PartId::kFootL].parent = &object3ds[PartId::kLegL];

	object3ds[PartId::kLegRootR].position = { jointDistance * 0.5f,0,0 };
	object3ds[PartId::kLegRootR].parent = &object3ds[PartId::kHip];

	object3ds[PartId::kLegR].position = { 0,-jointDistance,0 };
	object3ds[PartId::kLegR].scale = { 0.8f,2.0f,0.8f };
	object3ds[PartId::kLegR].parent = &object3ds[PartId::kLegRootR];

	object3ds[PartId::kFootR].position = { 0,-jointDistance * 0.8f,0 };
	object3ds[PartId::kFootR].parent = &object3ds[PartId::kLegR];

#pragma region �s��̌v�Z
	//�ˉe���e�ϊ�//
	XMMATRIX matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),		//�㉺��p45�x
		(float)win.width / win.height,	//�A�X�y�N�g��(��ʉ���/��ʏc��)
		0.1f, 1000.0f					//�O�[�A���[
	);

	//�����Ńr���[�ϊ��s��v�Z
	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -300);	//���_���W
	XMFLOAT3 target(0, 0, 0);	//�����_���W
	XMFLOAT3 up(0, 1, 0);		//������x�N�g��
	//�r���[�ϊ��s��쐬
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	float angle = 0.0f;	//�J�����̉�]�p
#pragma endregion

	//�l���������ނƎ����I�ɓ]�������
	constMapMaterial->color = XMFLOAT4(1, 0, 0, 0.5f);	//RGBA�Ŕ������̐�

	//�f�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;		//��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;	//�e�N�X�`�����W�X�^0��
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	//�萔�o�b�t�@0��
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//�萔�o�b�t�@�r���[
	rootParams[0].Descriptor.ShaderRegister = 0;					//�萔�o�b�t�@�ԍ�
	rootParams[0].Descriptor.RegisterSpace = 0;						//�f�t�H���g�l
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//�S�ẴV�F�[�_�[���猩����
	//�e�N�X�`�����W�X�^0��
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//���
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;			//�f�X�N���v�^�����W
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						//�f�X�N���v�^�����W��
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//�S�ẴV�F�[�_�[���猩����
	//�萔�o�b�t�@1��
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//���
	rootParams[2].Descriptor.ShaderRegister = 1;					//�萔�o�b�t�@�ԍ�
	rootParams[2].Descriptor.RegisterSpace = 0;						//�f�t�H���g�l
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//�S�ẴV�F�[�_�[���猩����

	TexMetadata metadate{};
	ScratchImage scratchImg{};
	//WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		L"Resources/mario.jpg",
		WIC_FLAGS_NONE,
		&metadate, scratchImg);

	ScratchImage mipChain{};
	//�~�b�v�}�b�v����
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadate = scratchImg.GetMetadata();
	}

	//�ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadate.format = MakeSRGB(metadate.format);

	//���_�f�[�^�\����
	struct Vertex
	{
		XMFLOAT3 pos;		//xyz���W
		XMFLOAT3 normal;	//�@���x�N�g��
		XMFLOAT2 uv;		//uv���W
	};

	// ���_�f�[�^
	Vertex vertices[] = {
		//	x		y		z	 �@��	u	v
		//�O
		{{ -5.0f, -5.0f, -5.0f }, {}, {0.0f,1.0f}},	// ����
		{{ -5.0f,  5.0f, -5.0f }, {}, {0.0f,0.0f}},	// ����
		{{  5.0f, -5.0f, -5.0f }, {}, {1.0f,1.0f}},	// �E��
		{{  5.0f,  5.0f, -5.0f }, {}, {1.0f,0.0f}},	// �E��

		//��
		{{ -5.0f, -5.0f, 5.0f }, {}, {0.0f,1.0f}},	// ����
		{{ -5.0f,  5.0f, 5.0f }, {}, {0.0f,0.0f}},	// ����
		{{  5.0f, -5.0f, 5.0f }, {}, {1.0f,1.0f}},	// �E��
		{{  5.0f,  5.0f, 5.0f }, {}, {1.0f,0.0f}},	// �E��

		 // ��
		{{-5.0f,-5.0f,-5.0f }, {}, {0.0f, 1.0f}},    // ����
		{{-5.0f,-5.0f, 5.0f }, {}, {0.0f, 0.0f}},    // ����
		{{-5.0f, 5.0f,-5.0f }, {}, {1.0f, 1.0f}},    // �E��
		{{-5.0f, 5.0f, 5.0f }, {}, {1.0f, 0.0f}},    // �E��

		// �E
		{{ 5.0f,-5.0f,-5.0f }, {}, {0.0f, 1.0f}},    // ����
		{{ 5.0f,-5.0f, 5.0f }, {}, {0.0f, 0.0f}},    // ����
		{{ 5.0f, 5.0f,-5.0f }, {}, {1.0f, 1.0f}},    // �E��
		{{ 5.0f, 5.0f, 5.0f }, {}, {1.0f, 0.0f}},    // �E��

		// ��
		{{-5.0f,-5.0f,-5.0f }, {}, {0.0f, 1.0f}},    // ����
		{{ 5.0f,-5.0f,-5.0f }, {}, {0.0f, 0.0f}},    // ����
		{{-5.0f,-5.0f, 5.0f }, {}, {1.0f, 1.0f}},    // �E��
		{{ 5.0f,-5.0f, 5.0f }, {}, {1.0f, 0.0f}},    // �E��

		// ��
		{{-5.0f, 5.0f,-5.0f }, {}, {0.0f, 1.0f}},    // ����
		{{ 5.0f, 5.0f,-5.0f }, {}, {0.0f, 0.0f}},    // ����
		{{-5.0f, 5.0f, 5.0f }, {}, {1.0f, 1.0f}},    // �E��
		{{ 5.0f, 5.0f, 5.0f }, {}, {1.0f, 0.0f}},    // �E��
	};

	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	// ���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp{}; // �q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p

	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // ���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�C���f�b�N�X�f�[�^
	unsigned short indices[] =
	{
		//�O
		0,1,2,	//�O�p�`1��
		2,1,3,	//�O�p�`2��
		//��
		5,4,6,	//�O�p�`3��
		5,6,7,	//�O�p�`4��
		//��
		8,9,10,	//�O�p�`5��
		10,9,11,//�O�p�`6��
		//�E
		13,12,14,	//�O�p�`7��
		13,14,15,	//�O�p�`8��
		//��
		16,17,18,	//�O�p�`9��
		18,17,19,	//�O�p�`10��
		//��
		21,20,22,	//�O�p�`11��
		21,22,23,	//�O�p�`12��
	};

	////�C���f�b�N�X�f�[�^(���̂��)
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

	//�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

	for (int i = 0; i < _countof(indices) / 3; i++)
	{//�O�p�`1���ƂɌv�Z���Ă���
		//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɂ����
		unsigned short index0 = indices[i * 3 + 0];
		unsigned short index1 = indices[i * 3 + 1];
		unsigned short index2 = indices[i * 3 + 2];

		//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);

		//p0��p1�x�N�g���Ap0��p2�x�N�g�����v�Z(�x�N�g���̌��Z)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		//�O�ς͗������琂���ȃx�N�g��
		XMVECTOR normal = XMVector3Cross(v1, v2);

		//���K��
		normal = XMVector3Normalize(normal);

		//���߂��@���𒸓_�f�[�^�ɑ��
		XMStoreFloat3(&vertices[index0].normal, normal);
		XMStoreFloat3(&vertices[index1].normal, normal);
		XMStoreFloat3(&vertices[index2].normal, normal);
	}

	// ���_�o�b�t�@�̐���
	ID3D12Resource* vertBuff = nullptr;
	result = directX.device->CreateCommittedResource(
		&heapProp, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	// GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// �S���_�ɑ΂���
	for (int i = 0; i < _countof(vertices); i++) {
		vertMap[i] = vertices[i]; // ���W���R�s�[
	}
	// �q���������
	vertBuff->Unmap(0, nullptr);

	// ���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	// GPU���z�A�h���X
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// ���_�o�b�t�@�̃T�C�Y
	vbView.SizeInBytes = sizeVB;
	// ���_1���̃f�[�^�T�C�Y
	vbView.StrideInBytes = sizeof(vertices[0]);

	// ���\�[�X�ݒ�
	/*D3D12_RESOURCE_DESC resDesc{};*/
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB; // ���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�C���f�b�N�X�o�b�t�@�̐���
	ID3D12Resource* indexBuff = nullptr;
	result = directX.device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);

	//�C���f�b�N�X�o�b�t�@���}�b�s���O
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//�S�C���f�b�N�X�ɑ΂���
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];	//�C���f�b�N�X���R�s�[
	}
	//�}�b�s���O����
	indexBuff->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�r���[�̍쐬
	D3D12_INDEX_BUFFER_VIEW ibView{};
	// GPU���z�A�h���X
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	//�C���f�b�N�X1���̃T�C�Y
	ibView.Format = DXGI_FORMAT_R16_UINT;
	// �C���f�b�N�X�o�b�t�@�̃T�C�Y
	ibView.SizeInBytes = sizeIB;

	//�e�N�X�`���o�b�t�@�ݒ�
	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC textureResouceDesc{};
	textureResouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResouceDesc.Format = metadate.format;
	textureResouceDesc.Width = metadate.width;	//��
	textureResouceDesc.Height = (UINT)metadate.height;	//����
	textureResouceDesc.DepthOrArraySize = (UINT16)metadate.arraySize;
	textureResouceDesc.MipLevels = (UINT16)metadate.mipLevels;
	textureResouceDesc.SampleDesc.Count = 1;

	//�e�N�X�`���o�b�t�@�̐���
	ID3D12Resource* texBuff = nullptr;

	result = directX.device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResouceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);

	//�S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < metadate.mipLevels; i++)
	{
		//�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,				//�S�̈�փR�s�[
			img->pixels,			//���f�[�^�A�h���X
			(UINT)img->rowPitch,	//1���C���T�C�Y
			(UINT)img->slicePitch	//�S�T�C�Y
		);
		assert(SUCCEEDED(result));
	}

	//�V�F�[�_�[���\�[�X�r���[�̍쐬
	//�V�F�[�_�[���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//�ݒ�\����
	srvDesc.Format = resDesc.Format;	//RGBA float
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;

	//�n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	directX.device->CreateShaderResourceView(texBuff, &srvDesc, directX.srvHandle);

	ID3DBlob* vsBlob = nullptr; // ���_�V�F�[�_�I�u�W�F�N�g
	ID3DBlob* psBlob = nullptr; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ID3DBlob* errorBlob = nullptr; // �G���[�I�u�W�F�N�g
	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"BasicVS.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);

	// �G���[�Ȃ�
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"BasicPS.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);

	// �G���[�Ȃ�
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		//xyz���W//
		{
		"POSITION",										//�Z�}���e�B�b�N��
		0,												//�����̃Z�}���e�B�b�N������Ƃ��g���C���f�b�N�X
		DXGI_FORMAT_R32G32B32_FLOAT,					//�v�f���ƃr�b�g����\��
		0,												//���̓X���b�g�C���f�b�N�X
		D3D12_APPEND_ALIGNED_ELEMENT,					//�f�[�^�̃I�t�Z�b�g�n(���͎̂����ݒ�)
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		//���̓f�[�^���
		0												//��x�ɕ`�悷��C���X�^���X��(0�ł悢)
		}, // (1�s�ŏ������ق������₷��)
		//���W�ȊO�ɐF�A�e�N�X�`��UV�Ȃǂ�n���ꍇ�͂���ɑ�����
		//�@���x�N�g��//
			{
				"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
			},
		//uv���W//
			{
				"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
				D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
			}
	};

	// �O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	// �V�F�[�_�[�̐ݒ�
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// �T���v���}�X�N�̐ݒ�
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

	// ���X�^���C�U�̐ݒ�
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;	// �w�ʂ��J�����O
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // �|���S�����h��Ԃ�(D3D12_FILL_MODE_WIREFRAME�ɂ���ƃ��C���[�t���[����)
	pipelineDesc.RasterizerState.DepthClipEnable = true; // �[�x�N���b�s���O��L����

	// �u�����h�X�e�[�g
	//pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
	//	= D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA�S�Ẵ`�����l����`��

	//�����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//RBGA�S�Ẵ`�����l����`��

	blenddesc.BlendEnable = true;					//�u�����h��L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//�\�[�X�̒l��100%�g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//�f�X�g�̒l��0%�g��

	//���Z����
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;			//�\�[�X�̃A���t�@�l
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	//1.0f-�\�[�X�̃A���t�@�l

	// ���_���C�A�E�g�̐ݒ�
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
	pipelineDesc.DepthStencilState.DepthEnable = true;
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	//�������݋���
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;	//��������΍��i
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;	//�[�x�l�t�H�[�}�b�g

	// ���̑��̐ݒ�
	pipelineDesc.NumRenderTargets = 1; // �`��Ώۂ�1��
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255�w���RGBA
	pipelineDesc.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	//�e�N�X�`���T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//���J��Ԃ��i�^�C�����O�j
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//�c�J��Ԃ��i�^�C�����O�j
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//���s�J��Ԃ��i�^�C�����O�j
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	//�{�[�_�[�̎��͍�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;					//�S�ă��j�A���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;									//�~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f;												//�~�b�v�}�b�v�ŏ��l
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;			//�s�N�Z���V�F�[�_����̂ݎg�p�\

	// ���[�g�V�O�l�`��
	ID3D12RootSignature* rootSignature;
	// ���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;					//���[�g�p�����[�^�̐擪�A�h���X
	rootSignatureDesc.NumParameters = _countof(rootParams);		//���[�g�p�����[�^��
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	// ���[�g�V�O�l�`���̃V���A���C�Y
	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = directX.device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();
	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipelineDesc.pRootSignature = rootSignature;

	// �p�C�v�����X�e�[�g�̐���
	ID3D12PipelineState* pipelineState = nullptr;
	result = directX.device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));
#pragma endregion
	//�Q�[�����[�v
	while (true)
	{
#pragma region �E�B���h�E���b�Z�[�W����
		if (PeekMessage(&win.msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&win.msg);	//�L�[���̓��b�Z�[�W�̏���
			DispatchMessage(&win.msg);	//�v���V�[�W���Ƀ��b�Z�[�W�𑗂�
		}


		//�A�v���P�[�V�������I���Ƃ���mwssage��WM_QUIT�ɂȂ�
		if (win.msg.message == WM_QUIT)
		{
			break;
		}
#pragma endregion

#pragma region DirectX���t���[������
		//DirectX���t���[���@��������
		key.InputUpdate();

#pragma region �s��̌v�Z
		if (key.IsKeyDown(DIK_D) || key.IsKeyDown(DIK_A))
		{
			if (key.IsKeyDown(DIK_D)) angle += XMConvertToRadians(10.0f);
			else if (key.IsKeyDown(DIK_A)) angle -= XMConvertToRadians(10.0f);

			//angle���W�A������Y������ɉ�]�B���a��-100
			eye.x = -300 * sinf(angle);
			eye.z = -300 * cosf(angle);
			//�r���[�ϊ��s��č쐬
			matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
		}

		//���W����
		if (key.IsKeyDown(DIK_UP) || key.IsKeyDown(DIK_DOWN) || key.IsKeyDown(DIK_RIGHT) || key.IsKeyDown(DIK_LEFT))
		{
			if (key.IsKeyDown(DIK_UP)) { object3ds[0].position.z += 1.0f; }
			else if (key.IsKeyDown(DIK_DOWN)) { object3ds[0].position.z -= 1.0f; }
			if (key.IsKeyDown(DIK_RIGHT)) { object3ds[0].position.x += 1.0f; }
			else if (key.IsKeyDown(DIK_LEFT)) { object3ds[0].position.x -= 1.0f; }
		}

		////�㔼�g��]
		//if (key.IsKeyDown(DIK_U) || key.IsKeyDown(DIK_I))
		//{
		//	if (key.IsKeyDown(DIK_U)) { object3ds[kChest].rotation.y += 0.05f; }
		//	else if (key.IsKeyDown(DIK_I)) { object3ds[kChest].rotation.y -= 0.05f; }
		//}

		////�����g��]
		//if (key.IsKeyDown(DIK_J) || key.IsKeyDown(DIK_K))
		//{
		//	if (key.IsKeyDown(DIK_J)) { object3ds[kHip].rotation.y += 0.05f; }
		//	else if (key.IsKeyDown(DIK_K)) { object3ds[kHip].rotation.y -= 0.05f; }
		//}

		armRotSpeed += 0.05f;
		fArmRotSpeed += 0.05f;

		object3ds[kLegRootL].rotation.x = sinf(armRotSpeed);
		object3ds[kLegRootR].rotation.x = sinf(-armRotSpeed);

		object3ds[kUpArmL].rotation.x = 0.5f * sinf(-armRotSpeed);
		object3ds[kUpArmR].rotation.x = 0.5f * sinf(armRotSpeed);

		//�O�r�̐U��
		object3ds[kForeArmL].rotation.x = 0.2f * (cosf(fArmRotSpeed) + 0.9f);
		object3ds[kForeArmR].rotation.x = 0.2f * (sinf(fArmRotSpeed) + 0.9f);

		/*object3ds[kForeArmL].position.z = 4.0f * cosf(-armRotSpeed);
		object3ds[kForeArmR].position.z = 4.0f * cosf(armRotSpeed);*/


		/*eye.z -= 1.0f;*/
		////�r���[�ϊ��s��č쐬
		//matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

		//rotation.x += 0.2f;
		//rotation.y += 0.2f;

		////�����ꂩ�̃L�[���������Ƃ�
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

		////���[���h�s��
		//XMMATRIX matScale;	//�X�P�[�����O�s��
		//matScale = XMMatrixScaling(scale.x, scale.y, scale.z);

		//XMMATRIX matRot = XMMatrixIdentity();	//��]�s��
		//matRot *= XMMatrixRotationZ(rotation.z);	//Z�������0�x��]���Ă���
		//matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));	//X�������15�x��]���Ă���
		//matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));	//Y�������30�x��]

		//XMMATRIX matTrans;	//���s�ړ��s��
		//matTrans = XMMatrixTranslation(position.x, position.y, position.z);

		//XMMATRIX matWorld = XMMatrixIdentity();	//�P�ʍs����
		//matWorld *= matScale;	//���[���h���W�ɃX�P�[�����O�𔽉f
		//matWorld *= matRot;	//���[���h���W�ɉ�]�𔽉f
		//matWorld *= matTrans;	//���[���h���W�ɕ��s�ړ��𔽉f

		////�萔�o�b�t�@�ɑ��M
		//directX.constMapTransform0->mat = matWorld * matView * matProjection;
#pragma endregion
		////���[���h�ϊ��s��
		//XMMATRIX matWorld1 = XMMatrixIdentity();

		////�e��ό`�s����v�Z
		//XMMATRIX matScale1 = XMMatrixScaling(1.0f, 1.0f, 1.0f);
		//XMMATRIX matRot1 = XMMatrixRotationY(XM_PI / 4.0f);
		//XMMATRIX matTrans1 = XMMatrixTranslation(-20.0f, 0, 0);

		////���[���h�s�������
		//matWorld1 = matTrans1 * matRot1 * matTrans1;

		////���[���h�A�r���[�A�ˉe�s����������ăV�F�[�_�[�ɓ]��
		//directX.constMapTransform1->mat = matWorld1 * matView * matProjection;

		//�l���������ނƎ����I�ɓ]�������
		constMapMaterial->color = XMFLOAT4(1, 1, 1, 1);	//RGBA�Ŕ������̐�

#pragma region �O���t�B�b�N�X�R�}���h
		// �o�b�N�o�b�t�@�̔ԍ����擾(2�Ȃ̂�0�Ԃ�1��)
		UINT bbIndex = directX.swapChain->GetCurrentBackBufferIndex();
		// 1.���\�[�X�o���A�ŏ������݉\�ɕύX
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = directX.backBuffers[bbIndex]; // �o�b�N�o�b�t�@���w��
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // �\����Ԃ���
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ�
		directX.commandList->ResourceBarrier(1, &barrierDesc);

		// 2.�`���̕ύX
		// �����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = directX.rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * directX.device->GetDescriptorHandleIncrementSize(directX.rtvHeapDesc.Type);
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
		directX.commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

		// 3.��ʃN���A R G B A
		FLOAT clearColor[] = { 0.1f,0.25f,0.5f,0.0f }; // ���ۂ��F
		directX.commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		directX.commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		//if (key[DIK_SPACE])
		//{
		//	FLOAT clearColor[] = { 0.0f,1.0f, 0.0f,0.0f }; // �Ԃ��ۂ��F
		//	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		//}

		// 4.�`��R�}���h��������
		// �r���[�|�[�g�ݒ�R�}���h
		D3D12_VIEWPORT viewport{};
		viewport.Width = win.width;
		viewport.Height = win.height;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;	//�ŏ��k�x
		viewport.MaxDepth = 1.0f;	//�ő�[�x
		// �r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
		directX.commandList->RSSetViewports(1, &viewport);

		// �V�U�[��`
		D3D12_RECT scissorRect{};
		scissorRect.left = 0; // �؂蔲�����W��
		scissorRect.right = scissorRect.left + win.width; // �؂蔲�����W�E
		scissorRect.top = 0; // �؂蔲�����W��
		scissorRect.bottom = scissorRect.top + win.height; // �؂蔲�����W��
		// �V�U�[��`�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
		directX.commandList->RSSetScissorRects(1, &scissorRect);

		// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
		directX.commandList->SetPipelineState(pipelineState);
		directX.commandList->SetGraphicsRootSignature(rootSignature);

		// �v���~�e�B�u�`��̐ݒ�R�}���h
		directX.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �O�p�`���X�g

		// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
		directX.commandList->IASetVertexBuffers(0, 1, &vbView);

		//�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
		directX.commandList->IASetIndexBuffer(&ibView);

		//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
		directX.commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
		//�V�F�[�_���\�[�X�r���[(SRV)�q�[�v�̐ݒ�R�}���h
		directX.commandList->SetDescriptorHeaps(1, &directX.srvHeap);
		//�V�F�[�_���\�[�X�r���[(SRV)�q�[�v�̐擪�n���h�����擾(SRV���w���Ă�͂�)
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = directX.srvHeap->GetGPUDescriptorHandleForHeapStart();
		//�V�F�[�_���\�[�X�r���[(SRV)�q�[�v�̐擪�ɂ���SRV�����[�g�p�����[�^1�Ԃɐݒ�
		directX.commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
		////0�Ԓ萔�o�b�t�@�r���[�iCRV�j�̐ݒ�R�}���h
		////���[�g�p�����[�^2�Ԃ�3D�ϊ��s��̒萔�o�b�t�@��n��
		//directX.commandList->SetGraphicsRootConstantBufferView(2, directX.constBuffTransform0->GetGPUVirtualAddress());
		//// �`��R�}���h
		//directX.commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0); // �C���f�b�N�X�o�b�t�@���g���ĕ`��

		////1�Ԓ萔�o�b�t�@�r���[�iCRV�j�̐ݒ�R�}���h
		////���[�g�p�����[�^2�Ԃ�3D�ϊ��s��̒萔�o�b�t�@��n��
		//directX.commandList->SetGraphicsRootConstantBufferView(2, directX.constBuffTransform1->GetGPUVirtualAddress());
		//// �`��R�}���h
		//directX.commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0); // �C���f�b�N�X�o�b�t�@���g���ĕ`��

		for (int i = kChest; i < _countof(object3ds); i++)
		{
			if (i != kLegRootL && i != kLegRootR)
			{
				object3ds[i].DrawObject3d(&object3ds[i], directX.commandList, vbView, ibView, _countof(indices));
			}
		}
		// 4.�`��R�}���h�����܂�

		// 5.���\�[�X�o���A��߂�
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ���
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // �\����Ԃ�
		directX.commandList->ResourceBarrier(1, &barrierDesc);
#pragma endregion

		// ���߂̃N���[�Y
		result = directX.commandList->Close();
		assert(SUCCEEDED(result));
		// �R�}���h���X�g�̎��s
		ID3D12CommandList* commandLists[] = { directX.commandList };
		directX.commandQueue->ExecuteCommandLists(1, commandLists);
#pragma endregion

		// ��ʂɕ\������o�b�t�@���t���b�v(���\�̓��ւ�)
		result = directX.swapChain->Present(1, 0);
		assert(SUCCEEDED(result));

		// �R�}���h�̎��s������҂�
		directX.commandQueue->Signal(fence, ++fenceVal);
		if (fence->GetCompletedValue() != fenceVal) {
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
		// �L���[���N���A
		result = directX.commandAllocator->Reset();
		assert(SUCCEEDED(result));
		// �ĂуR�}���h���X�g�𒙂߂鏀��
		result = directX.commandList->Reset(directX.commandAllocator, nullptr);
		assert(SUCCEEDED(result));

		//DirectX���t���[���@�����܂�
	}
#pragma region WindowsAPI��n��
	//�����N���X�͎g��Ȃ��̂œo�^��������
	UnregisterClass(win.w.lpszClassName, win.w.hInstance);
#pragma endregion

	return 0;
}