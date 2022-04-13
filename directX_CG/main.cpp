#include <windows.h>

//Windows�A�v���ł̃G���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//�R���\�[���ւ̕����o��
	OutputDebugStringA("Hello,DirectX!!\n");

	const int window_width = 1280;
	const int window_height = 720;

	//�E�B���h�E�N���X�̐������o�^
	WNDCLASSEX w {};

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;	//�R�[���o�b�N�֐��̎w��
	w.lpszClassName = L"DX12Sample";			//�A�v���P�[�V�����N���X��
	w.hInstance = GetModuleHandle(nullptr);		//�n���h���̎擾
	w.hCursor = LoadCursor(NULL, IDC_ARROW);	//�J�[�\���w��

	RegisterClassEx(&w);	//�A�v���P�[�V�����N���X�i�E�B���h�E�N���X�̎w���OS�ɓ`����j

	RECT wrc = { 0,0,window_width,window_height };	//�E�B���h�E�T�C�Y�����߂�

	//�֐����g���ăE�B���h�E�̃T�C�Y��␳����
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//�E�B���h�E�I�u�W�F�N�g�̐���
	HWND hwnd = CreateWindow(
		w.lpszClassName,		//�N���X���w��
		L"DX12�e�X�g",			//�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,	//�^�C�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT,			//�\��x���W��OS�ɂ��C��
		CW_USEDEFAULT,			//�\��y���W��OS�ɂ��C��
		wrc.right - wrc.left,	//�E�B���h�E��
		wrc.bottom - wrc.top,	//�E�B���h�E��
		nullptr,				//�e�E�B���h�E�n���h��
		nullptr,				//���j���[�n���h��
		w.hInstance,			//�Ăяo���A�v���P�[�V�����n���h��
		nullptr					//�ǉ��p�����[�^�[
	);

	return 0;
}

//�E�B���h�v���V�[�W��
//�ʓ|�����Ǐ����Ȃ��Ⴂ���Ȃ��֐�
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		//�E�B���h�E���j�����ꂽ��Ă΂��
	case WM_DESTROY:
			PostQuitMessage(0);	//OS�ɑ΂��āu�������̃A�v���͏I���v�Ɠ`����
			return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);	//����̏������s��
}
