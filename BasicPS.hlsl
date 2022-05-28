#include "Basic.hlsli"

float4 main() : SV_TARGET
{
	return color;
}

Texture2D<float4>tex : register(t0);	//0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);	//0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	float4 texcolor = float4(tex.Sample(smp,input.uv)) /** color*/;	//定数バッファに送った色変えると反映される
	//return float4(1,1,1,1);	//白く塗りつぶす
	float3 light = normalize(float3(1,-1,1));						//右下奥　向きのライト
	float diffuse = saturate(dot(-light, input.normal));			//diffuseを[0,1]の範囲にClampする
	float brightness = diffuse + 0.3f;								//アンビエント項を0.3として計算
	return float4(texcolor.rgb * brightness, texcolor.a) * color;	//輝度をRGBに代入して出力
}