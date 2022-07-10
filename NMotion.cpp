#include "NMotion.h"

void Motion::Initialize(ID3D12Device* device)
{
	pad.Initialize();
	object3d.InitializeObject3d(device);
}

void Motion::Update(XMMATRIX matView, XMMATRIX matProjection)
{
}

void Motion::Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW vbView, D3D12_INDEX_BUFFER_VIEW ibView, int indicesSize)
{
	object3d.DrawObject3d(commandList, vbView, ibView, indicesSize);
	floor.DrawObject3d(commandList, vbView, ibView, indicesSize);	//è∞
}

float Motion::Clamp(float value, float min, float max)
{
	if (value < min)
	{
		return min;
	}
	else if (value > max)
	{
		return max;
	}
	return value;
}