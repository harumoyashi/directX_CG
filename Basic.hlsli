//定数バッファ用構造体
cbuffer ConstBufferDataMaterial : register(b0)
{
	float4 color;	//色(RGBA)
};

//頂点データの出力構造体
//(頂点シェーダーからピクセルシェーダーへのやりとりに使用する)
struct VSOutput
{
	//システム用頂点座標
	float4 svpos:SV_POSITION;
	//法線ベクトル
	float3 normal :NORMAL;
	//uv値
	float2 uv:TEXCOORD;
	//色
	float3 color:COLOR;
};

//3D変換行列
cbuffer ConstBufferDataTransform : register(b1)
{
	matrix mat;	//3D変換行列
};