#pragma pack_matrix( row_major )

struct PointVertexIn
{
    float3 PosL : POSITION;
    float2 Size : RENDERSIZE;
    float4 Color : TEXCOORD;
    int TexIndex : TEXINDEX;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 Color : TEXCOORD0;
    nointerpolation int TexIndex : TEXINDEX0;
};

struct TextureInfo
{
    float2 StartUV;
    float2 EndUV;
    uint   TextureIndex;
    uint   Pad0;
    uint   Pad1;
    uint   Pad2;
};

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6);

cbuffer cbPass : register(b0)
{
    float4x4 gView;
    float4x4 gProj;
    float4x4 gViewProj;
    float4x4 gOrthoMatrix;
    float2 gMousePos;
    uint2 gRenderTargetSize;
};

StructuredBuffer<TextureInfo> gMainTextureInfos : register(t0);
Texture2DArray gMainTextures : register(t1);


float4 GetTextureSample(SamplerState samp, int index, float2 uv)
{
    float4 result = { 0, 0, 0, 0 };
    int currIndex = index & 0x0000ffff;
    TextureInfo currTextureInfo = gMainTextureInfos[currIndex];
    
    float2 targetUV = currTextureInfo.StartUV;
    float2 scale = currTextureInfo.EndUV - targetUV;
    targetUV = targetUV + (uv * scale);
    
    result = gMainTextures.Sample(samp, float3(targetUV, currTextureInfo.TextureIndex));
    
    return result;
}