#include "NMotion.h"

void Motion::Initialize(ID3D12Device* device)
{
	for (int i = 0; i < kNumPartId; i++)
	{
		object3d[i].InitializeObject3d(device);
	}
	floor.InitializeObject3d(device);

	const float jointDistance = 15.0f;

	object3d[PartId::kRoot].position = { 0,0,0 };

	object3d[PartId::kSpine].position = { 0,jointDistance,0 };
	object3d[PartId::kSpine].parent = &object3d[PartId::kRoot];

	object3d[PartId::kChest].position = { 0,0,0 };
	object3d[PartId::kChest].parent = &object3d[PartId::kSpine];

	object3d[PartId::kHead].position = { 0,jointDistance,0 };
	object3d[PartId::kHead].scale = { 1.5f,1.5f,1.5f };
	object3d[PartId::kHead].parent = &object3d[PartId::kSpine];

	object3d[PartId::kUpArmL].position = { -jointDistance * 0.8f,-2.0f,0 };
	object3d[PartId::kUpArmL].rotation = { 0,0,-0.05f };
	object3d[PartId::kUpArmL].scale = { 0.5f,1.3f,0.5f };
	object3d[PartId::kUpArmL].parent = &object3d[PartId::kChest];

	object3d[PartId::kForeArmL].position = { 0,-jointDistance * 0.8f,0 };
	object3d[PartId::kForeArmL].scale = { 1.0f,1.0f,1.0f };
	object3d[PartId::kForeArmL].parent = &object3d[PartId::kUpArmL];

	object3d[PartId::kHandL].position = { 0,-jointDistance * 0.7f,0 };
	object3d[PartId::kHandL].scale = { 1.0f,0.5f,1.0f };
	object3d[PartId::kHandL].parent = &object3d[PartId::kForeArmL];

	object3d[PartId::kUpArmR].position = { jointDistance * 0.8f,-2.0f,0 };
	object3d[PartId::kUpArmR].rotation = { 0,0,0.05f };
	object3d[PartId::kUpArmR].scale = { 0.5f,1.3f,0.5f };
	object3d[PartId::kUpArmR].parent = &object3d[PartId::kChest];

	object3d[PartId::kForeArmR].position = { 0,-jointDistance * 0.8f,0 };
	object3d[PartId::kForeArmR].scale = { 1.0f,1.0f,1.0f };
	object3d[PartId::kForeArmR].parent = &object3d[PartId::kUpArmR];

	object3d[PartId::kHandR].position = { 0,-jointDistance * 0.7f,0 };
	object3d[PartId::kHandR].scale = { 1.0f,0.5f,1.0f };
	object3d[PartId::kHandR].parent = &object3d[PartId::kForeArmR];

	object3d[PartId::kWaist].position = { 0,-jointDistance,0 };
	object3d[PartId::kWaist].parent = &object3d[PartId::kSpine];

	object3d[PartId::kHip].position = { 0,-jointDistance,0 };
	object3d[PartId::kHip].parent = &object3d[PartId::kWaist];

	object3d[PartId::kUpLegRootL].position = { -jointDistance * 0.5f,0,0 };
	object3d[PartId::kUpLegRootL].parent = &object3d[PartId::kHip];

	object3d[PartId::kUpLegL].position = { 0,-jointDistance,0 };
	object3d[PartId::kUpLegL].scale = { 0.8f,2.0f,0.8f };
	object3d[PartId::kUpLegL].parent = &object3d[PartId::kUpLegRootL];

	object3d[PartId::kKneeL].position = { 0,-jointDistance * 0.3f,0 };
	object3d[PartId::kKneeL].scale = { 1.0f,0.5f,1.0f };
	object3d[PartId::kKneeL].parent = &object3d[PartId::kUpLegL];

	object3d[PartId::kLowLegL].position = { 0,-jointDistance * 0.8f,0 };
	object3d[PartId::kLowLegL].scale = { 1.0f,2.0f,1.0f };
	object3d[PartId::kLowLegL].parent = &object3d[PartId::kKneeL];

	object3d[PartId::kFootL].position = { 0,-jointDistance * 0.6f,-2.0f };
	object3d[PartId::kFootL].scale = { 1.0f,0.5f,1.3f };
	object3d[PartId::kFootL].parent = &object3d[PartId::kLowLegL];

	object3d[PartId::kUpLegRootR].position = { jointDistance * 0.5f,0,0 };
	object3d[PartId::kUpLegRootR].parent = &object3d[PartId::kHip];

	object3d[PartId::kUpLegR].position = { 0,-jointDistance,0 };
	object3d[PartId::kUpLegR].scale = { 0.8f,2.0f,0.8f };
	object3d[PartId::kUpLegR].parent = &object3d[PartId::kUpLegRootR];

	object3d[PartId::kKneeR].position = { 0,-jointDistance * 0.3f,0 };
	object3d[PartId::kKneeR].scale = { 1.0f,0.5f,1.0f };
	object3d[PartId::kKneeR].parent = &object3d[PartId::kUpLegR];

	object3d[PartId::kLowLegR].position = { 0,-jointDistance * 0.8f,0 };
	object3d[PartId::kLowLegR].scale = { 1.0f,2.0f,1.0f };
	object3d[PartId::kLowLegR].parent = &object3d[PartId::kKneeR];

	object3d[PartId::kFootR].position = { 0,-jointDistance * 0.6f,-2.0f };
	object3d[PartId::kFootR].scale = { 1.0f,0.5f,1.3f };
	object3d[PartId::kFootR].parent = &object3d[PartId::kLowLegR];

	/*object3d[PartId::kFloor].position = { 0,-920.0f,0 };
	object3d[PartId::kFloor].scale = { 30.0f,1.0f,30.0f };
	object3d[PartId::kFloor].parent = &object3d[PartId::kRoot];*/

	floor.position = { 0,-90.0f,0 };
	floor.scale = { 30.0f,1.0f,30.0f };
}

void Motion::Update(XMMATRIX matView, XMMATRIX matProjection)
{
	rotSpeed = timer;	//基本の回転はタイマーと同じ値
	legRotSpeed = maxHalfTimer * EaseIn(halfTimer / maxHalfTimer) + PI / 2;	//脚の付け根はイージングかけて、半分ずらしておく
	//一回脚降ったら符号反転させて脚の振り逆に
	if (timer - speed < maxHalfTimer)
	{
		legRotSpeed = -legRotSpeed;
	}

#pragma region 回転処理
	if (key.IsKeyDown(DIK_RETURN))
	{
		//二の腕の振り
		object3d[kUpArmL].rotation.x = (0.5f/*+speed*2*/) * sinf(rotSpeed);
		object3d[kUpArmR].rotation.x = 0.5f * sinf(-rotSpeed);
		//前腕の振り
		object3d[kForeArmL].rotation.x = 0.2f * (sinf(rotSpeed) + 0.9f);
		object3d[kForeArmR].rotation.x = 0.2f * (sinf(-rotSpeed) + 0.9f);
		//脚付け根の回転
		object3d[kUpLegRootL].rotation.x = 0.8f * sinf(legRotSpeed) + 0.2f;
		object3d[kUpLegRootR].rotation.x = 0.8f * sinf(-legRotSpeed) + 0.2f;
		//膝の回転(膝の描画なし)
		object3d[kKneeL].rotation.x = 0.5f * sinf(-legRotSpeed) - 0.5f;
		object3d[kKneeR].rotation.x = 0.5f * sinf(legRotSpeed) - 0.5f;
		////足の回転
		//object3d[kFootL].rotation.x = 0.1f * sinf(rotSpeed) + 0.2f;
		//object3d[kFootR].rotation.x = 0.1f * sinf(-rotSpeed) + 0.2f;
		//重心移動
		object3d[kSpine].position.y = 0.7f * -sinf(rotSpeed * 2.0f) + 2.0f;
		object3d[kSpine].rotation.x = 0.03f * -sinf(rotSpeed * 2.0f) - 0.1f;
		//胸の捻り
		object3d[kChest].rotation.y = 0.1f * sinf(rotSpeed);
		////おしりの捻り
		//object3d[kHip].rotation.y = 0.1f * -sinf(rotSpeed);
	}
#pragma endregion
	for (size_t i = 0; i < kNumPartId; i++)
	{
		object3d[i].UpdateObject3d(matView, matProjection);
	}
	floor.UpdateObject3d(matView, matProjection);	//床
}

void Motion::Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW vbView, D3D12_INDEX_BUFFER_VIEW ibView, int indicesSize)
{
	for (int i = kChest; i < kNumPartId; i++)
	{
		object3d[i].DrawObject3d(commandList, vbView, ibView, indicesSize);
	}
	floor.DrawObject3d(commandList, vbView, ibView, indicesSize);	//床
}

void Motion::StartTimer()
{
	if (key.IsKeyDown(DIK_RETURN))
	{
		if (timer > maxTimer)
		{
			timer = 0;	//タイマーリセット
		}
		timer += speed;
		//ハーフタイマー
		if (halfTimer > maxHalfTimer)
		{
			halfTimer = 0;
		}
		halfTimer += speed;
	}
	else
	{
		//離したらタイマーリセット
		timer = 0;
		halfTimer = 0;
	}
}

void Motion::RotationKey()
{
	////座標操作
	//if (key.IsKeyDown(DIK_UP) || key.IsKeyDown(DIK_DOWN) || key.IsKeyDown(DIK_RIGHT) || key.IsKeyDown(DIK_LEFT))
	//{
	//	if (key.IsKeyDown(DIK_UP)) { object3d[0].position.y += 1.0f; }
	//	else if (key.IsKeyDown(DIK_DOWN)) { object3d[0].position.y -= 1.0f; }
	//	if (key.IsKeyDown(DIK_RIGHT)) { object3d[0].position.x += 1.0f; }
	//	else if (key.IsKeyDown(DIK_LEFT)) { object3d[0].position.x -= 1.0f; }
	//}

	//上半身回転
	if (key.IsKeyDown(DIK_U) || key.IsKeyDown(DIK_I))
	{
		if (key.IsKeyDown(DIK_U)) { object3d[kChest].rotation.y += 0.05f; }
		else if (key.IsKeyDown(DIK_I)) { object3d[kChest].rotation.y -= 0.05f; }
	}

	//下半身回転
	if (key.IsKeyDown(DIK_J) || key.IsKeyDown(DIK_K))
	{
		if (key.IsKeyDown(DIK_J)) { object3d[kHip].rotation.y += 0.05f; }
		else if (key.IsKeyDown(DIK_K)) { object3d[kHip].rotation.y -= 0.05f; }
	}

	////スピード変更
	//if (key.IsKeyTrigger(DIK_UP) || key.IsKeyTrigger(DIK_DOWN))
	//{
	//	/*if (speed < 0.2f && speed >= 0.05f)
	//	{*/
	//		if (key.IsKeyTrigger(DIK_UP)) speed += 0.01f;
	//		else if (key.IsKeyTrigger(DIK_DOWN)) speed -= 0.01f;
	//	/*}*/
	//}
}
