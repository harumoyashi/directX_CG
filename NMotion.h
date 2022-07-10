#pragma once
#include "NDirectXInput.h"
#include "NInputPad.h"
#include "NObject.h"

class Motion
{
private:
	DirectXInput key;	//�L�[�{�[�h
	InputPad pad;

	Vector2 frontVec = { 0,1 };

public:
	//3D�I�u�W�F�N�g�̔z��
	Object3d object3d;

	Object3d floor;

public:
	//������
	void Initialize(ID3D12Device* device);
	//�X�V
	void Update(XMMATRIX matView, XMMATRIX matProjection);
	//�`��
	void Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW vbView, D3D12_INDEX_BUFFER_VIEW ibView, int indicesSize);
	
	//臒l�����Ȃ��悤��
	float Clamp(float value, float min, float max);
};
