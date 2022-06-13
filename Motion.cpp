#include "Motion.h"

void Motion::StartButton()
{
	/*if (key.IsKeyDown(DIK_SPACE))
	{*/
		rotSpeed += 0.05f;

		if (sinf(rotSpeed) <= 0.0f)
		{
			lKneeRotSpeed += 0.1f;
		}
		else
		{
			rKneeRotSpeed += 0.1f;
		}
	//}
	/*else
	{
		rotSpeed = PI;
	}*/
}

void Motion::SetRotPara()
{
	//回転情報を変数に格納//
	//二の腕の振り
	rot[kUpArmL].x = 0.5f * sinf(-rotSpeed);
	rot[kUpArmR].x = 0.5f * sinf(rotSpeed);
	//前腕の振り
	rot[kForeArmL].x = 0.2f * (cosf(rotSpeed) + 0.9f);
	rot[kForeArmR].x = 0.2f * (sinf(rotSpeed) + 0.9f);

	//脚付け根の回転
	rot[kUpLegRootL].x = /*EaseInSine(*/0.3f * sinf(rotSpeed) + 0.2f/*)*/;
	rot[kUpLegRootR].x = 0.3f * sinf(-rotSpeed) + 0.2f;
	////膝の回転(膝の描画なし)
	//rot[kKneeL].x = 0.5f * sinf(lKneeRotSpeed + 1.0f) - 0.5f;
	//rot[kKneeR].x = 0.5f * sinf(rKneeRotSpeed + 1.0f) - 0.5f;
	////足の回転
	//rot[kFootL].x = 0.1f * sinf(rotSpeed) + 0.2f;
	//rot[kFootR].x = 0.1f * sinf(-rotSpeed) + 0.2f;

	//重心移動
	spineTrans.y = 0.5f * -sinf(rotSpeed * 2.0f) + 2.0f;
	rot[kSpine].x = 0.02f * -sinf(rotSpeed * 2.0f) - 0.03f;

	//胸の捻り
	rot[kChest].y = 0.1f * sinf(rotSpeed);
	//おしりの捻り
	rot[kHip].y = 0.1f * -sinf(rotSpeed);
}

void Motion::SetRot()
{
	//格納しておいた回転情報を代入//
	//二の腕の振り
	object3ds[kUpArmL].rotation.x = rot[kUpArmL].x;
	object3ds[kUpArmR].rotation.x = rot[kUpArmR].x;
	//前腕の振り
	object3ds[kForeArmL].rotation.x = rot[kForeArmL].x;
	object3ds[kForeArmR].rotation.x = rot[kForeArmR].x;

	//脚付け根の回転
	object3ds[kUpLegRootL].rotation.x = rot[kUpLegRootL].x;
	object3ds[kUpLegRootR].rotation.x = rot[kUpLegRootR].x;
	//膝の回転(膝の描画なし)
	object3ds[kKneeL].rotation.x = rot[kKneeL].x;
	object3ds[kKneeR].rotation.x = rot[kKneeR].x;
	////足の回転
	//object3ds[kFootL].rotation.x = objectRot[kFootL].x;
	//object3ds[kFootR].rotation.x = objectRot[kFootR].x;

	//重心移動
	object3ds[kSpine].position.y = spineTrans.y;
	object3ds[kSpine].rotation.x = rot[kSpine].x;

	//胸の捻り
	object3ds[kChest].rotation.y = rot[kChest].y;
	//おしりの捻り
	object3ds[kHip].rotation.y = rot[kHip].y;
}
