#include "Basic.hlsli"

//float4 main() : SV_TARGET
//{
//	return color;
//}

Texture2D<float4>tex:register(t0);	//0番スロットに設定されたテクスチャ
SampleState smp : register(s0);		//0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	return float4(tex.Sample(smp,input.uv));
}