#pragma once
#include "DirectXInput.h"
#include <vector>
#include "Easing.h"
#include "Object3d.h"

class Motion
{
private:
	double rotSpeed;
	double armRotSpeed;
	double legRootRotSpeed;
	double lKneeRotSpeed;
	double rKneeRotSpeed;
	double footRotSpeed;

public:
	void StartButton();
	void SetRotPara();
	void SetRot();
	int motionTimer = 0;

	XMFLOAT3 rot[kNumPartId];
	XMFLOAT3 spineTrans;
private:
	Object3d object3ds[kNumPartId];
	DirectXInput key;

};