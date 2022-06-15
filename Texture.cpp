#include "Texture.h"

void Texture::Initialize()
{
	//						x		y		z		u		v
	vertices.push_back({ { -50.0f, -50.0f, 0.0f }, {0.0f,1.0f} });	// ����
	vertices.push_back({ { -50.0f, 50.0f , 0.0f }, {0.0f,0.0f} });	// ����
	vertices.push_back({ { 50.0f , -50.0f, 0.0f }, {1.0f,1.0f} });	// �E��
	vertices.push_back({ { 50.0f , 50.0f , 0.0f }, {1.0f,0.0f} });	// �E��
}

void Texture::Lord(const wchar_t* name_format)
{
	result = LoadFromWICFile(
		name_format,
		WIC_FLAGS_NONE,
		&metadate, scratchImg);
}

void Texture::CreateMipMap()
{
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadate = scratchImg.GetMetadata();
	}
}

void Texture::SetSRGB()
{
	metadate.format = MakeSRGB(metadate.format);
}

void Texture::SetTexBuff()
{
	//�e�N�X�`���o�b�t�@�ݒ�
	//�q�[�v�ݒ�
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//���\�[�X�ݒ�
	textureResouceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResouceDesc.Format = metadate.format;
	textureResouceDesc.Width = metadate.width;	//��
	textureResouceDesc.Height = (UINT)metadate.height;	//����
	textureResouceDesc.DepthOrArraySize = (UINT16)metadate.arraySize;
	textureResouceDesc.MipLevels = (UINT16)metadate.mipLevels;
	textureResouceDesc.SampleDesc.Count = 1;
}

void Texture::CreateTexBuff(ID3D12Device* device)
{
	//�e�N�X�`���o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResouceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);
}

void Texture::SetMipMap()
{
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
}
