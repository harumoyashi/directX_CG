#pragma once
#include <Windows.h>
#include <Xinput.h>

#include "NVector2.h"

//xinput.lib ���C���|�[�g
#pragma comment (lib,"xinput.lib")

class InputPad
{
public:
    //XINPUT_STATE �\���̂̃C���X�^���X���쐬
    XINPUT_STATE state;

    //�ڑ�����Ă邩
    bool isConnect = false;



public:
    //������
    void Initialize();
    //�X�V
    void Update();

    //�������ςȂ�
    bool IsButtonDown(UINT button);    //UINT�̓r�b�g���w�肵����^���R�ۂ�
    //�������u��
    bool IsButtonTrigger(UINT button);
    //�����Ă�Ƃ�
    bool IsButtonReleased(UINT button);
    //�������u��
    bool IsButtonReleaseTrigger(UINT button);

    //���g���K�[�̉������݋�擾
    int GetLTrigger(BYTE leftTrigger);
    //�E�g���K�[�̉������݋�擾
    int GetRTrigger(BYTE rightTrigger);

    //�f�b�h�]�[���̐ݒ�
    void SetDeadZone();

    //���X�e�B�b�N�̌X����擾
    Vector2 GetLStick();
    //�E�X�e�B�b�N�̌X����擾
    Vector2 GetRStick();
};