//�萔�o�b�t�@�p�\����
cbuffer ConstBufferDataMaterial : register(b0)
{
	float4 color;	//�F(RGBA)
};

//���_�f�[�^�̏o�͍\����
//(���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂��Ƃ�Ɏg�p����)
struct VSOutput
{
	//�V�X�e���p���_���W
	float4 svpos:SV_POSITION;
	//uv�l
	float2 uv:TEXCOORD;
};