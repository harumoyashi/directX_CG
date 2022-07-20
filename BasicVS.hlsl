#include "Basic.hlsli"

//float4 main(float4 pos : POSITION) : SV_POSITION
//{
//	////��]
//	//float4 pos2 = pos;	//�R�s�[
//	//const float PI = 3.14159;	//��
//	//float angle = PI * 0.25f;	//��]�p(���W�A��)
//	//pos2.x = pos.x * cos(angle) - pos.y * sin(angle);
//	//pos2.y = pos.x * sin(angle) + pos.y * cos(angle);
//	//return pos2;
//	return pos + float4(-0.2f,-0.2f,1,1);
//}

VSOutput main(float4 pos:POSITION,float3 normal : NORMAL, float2 uv : TEXCOORD,float3 color : COLOR)
{
	VSOutput output;	//�s�N�Z���V�F�[�_�[�ɓn���l
	/*output.svpos = pos;*/
	output.svpos = mul(mat, pos);	//���W�ɍs�����Z
	output.normal = normal;
	output.uv = uv;
	output.color = color;
	return output;
}