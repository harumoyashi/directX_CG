#pragma once
#include <Windows.h>
#include <Xinput.h>

#include "NVector2.h"

//xinput.lib をインポート
#pragma comment (lib,"xinput.lib")

class InputPad
{
public:
    //XINPUT_STATE 構造体のインスタンスを作成
    XINPUT_STATE state;

    //接続されてるか
    bool isConnect = false;



public:
    //初期化
    void Initialize();
    //更新
    void Update();

    //押しっぱなし
    bool IsButtonDown(UINT button);    //UINTはビット数指定したら型自由ぽい
    //押した瞬間
    bool IsButtonTrigger(UINT button);
    //離してるとき
    bool IsButtonReleased(UINT button);
    //離した瞬間
    bool IsButtonReleaseTrigger(UINT button);

    //左トリガーの押し込み具合取得
    int GetLTrigger(BYTE leftTrigger);
    //右トリガーの押し込み具合取得
    int GetRTrigger(BYTE rightTrigger);

    //デッドゾーンの設定
    void SetDeadZone();

    //左スティックの傾き具合取得
    Vector2 GetLStick();
    //右スティックの傾き具合取得
    Vector2 GetRStick();
};