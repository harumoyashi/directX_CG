#include "Basic.hlsli"

float4 main() : SV_TARGET
{
	return color;
}

Texture2D<float4>tex:register(t0);	//0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);	//0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	//return float4(tex.Sample(smp,input.uv)) /** color*/;	//定数バッファに送った色変えると反映される
	//return float4(1,1,1,1);	//白く塗りつぶす
	return float4(input.normal,1);	//RGBをそれぞれ法線のXYZ,Aを1で出力
}