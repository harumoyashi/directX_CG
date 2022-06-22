#pragma once

#include "NSprite.h"
#include <Windows.h>
#include <string>

/// <summary>
/// �f�o�b�O�p�����\��
/// </summary>
class DebugText {
public:
	// �f�o�b�O�e�L�X�g�p�̃e�N�X�`���ԍ����w��
	static const int kMaxCharCount = 512; // �ő啶����
	static const int kFontWidth = 9;      // �t�H���g�摜��1�������̉���
	static const int kFontHeight = 18;    // �t�H���g�摜��1�������̏c��
	static const int kFontLineCount = 14; // �t�H���g�摜��1�s���̕�����
	static const int kBufferSize = 512;   // �����t��������W�J�p�o�b�t�@�T�C�Y

	/// <summary>
	/// �V���O���g���C���X�^���X�̎擾
	/// </summary>
	/// <returns>�V���O���g���C���X�^���X</returns>
	static DebugText* GetInstance();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// ������ǉ�
	/// </summary>
	/// <param name="text">������</param>
	/// <param name="x">�\�����WX</param>
	/// <param name="y">�\�����WY</param>
	/// <param name="scale">�{��</param>
	void Print(const std::string& text, float x, float y, float scale = 1.0f);

	/// <summary>
	/// �����t��������ǉ�
	/// </summary>
	/// <param name="fmt">�����t��������</param>
	void Printf(const char* fmt, ...);

	/// <summary>
	/// �����t���R���\�[���o��
	/// </summary>
	/// <param name="fmt">�����t��������</param>
	void ConsolePrintf(const char* fmt, ...);

	/// <summary>
	/// �`��t���b�V��
	/// </summary>
	/// <param name="cmdList">�`��R�}���h���X�g</param>
	void DrawAll(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �`����W�̎w��
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	void SetPos(float x, float y) {
		posX_ = x;
		posY_ = y;
	}

	/// <summary>
	/// �`��{���̎w��
	/// </summary>
	/// <param name="scale">�{��</param>
	void SetScale(float scale) { scale_ = scale; }

private:
	// �e�N�X�`���n���h��
	uint32_t textureHandle_ = 0;
	// �X�v���C�g�f�[�^�̔z��
	Sprite* spriteDatas_[kMaxCharCount] = {};
	// �X�v���C�g�f�[�^�z��̓Y�����ԍ�
	int spriteIndex_ = 0;

	float posX_ = 0.0f;
	float posY_ = 0.0f;
	float scale_ = 1.0f;
	// �����t��������W�J�p�o�b�t�@
	char buffer[kBufferSize];

	DebugText();
	~DebugText();
	DebugText(const DebugText&) = delete;
	DebugText& operator=(const DebugText&) = delete;
	void NPrint(int len, const char* text);
};


