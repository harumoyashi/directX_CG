#include "Basic.hlsli"

//float4 main(float4 pos : POSITION) : SV_POSITION
//{
//	////回転
//	//float4 pos2 = pos;	//コピー
//	//const float PI = 3.14159;	//π
//	//float angle = PI * 0.25f;	//回転角(ラジアン)
//	//pos2.x = pos.x * cos(angle) - pos.y * sin(angle);
//	//pos2.y = pos.x * sin(angle) + pos.y * cos(angle);
//	//return pos2;
//	return pos + float4(-0.2f,-0.2f,1,1);
//}

VSOutput main(float4 pos:POSITION, float2 uv : TEXCOORD)
{
	VSOutput output;	//ピクセルシェーダーに渡す値
	/*output.svpos = pos;*/
	output.svpos = mul(mat, pos);	//座標に行列を乗算
	output.uv = uv;
	return output;
}