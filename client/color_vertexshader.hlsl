#pragma pack_matrix( row_major )

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4 color;
};

struct VS_INPUT
{
    float2 inPos : POSITION;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float4 outColor : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outPosition = mul(float4(input.inPos, 0.0f, 1.0f), wvpMatrix);
    output.outColor = color;
    return output;
}