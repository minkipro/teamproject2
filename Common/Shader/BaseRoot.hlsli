#pragma pack_matrix( row_major )

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

Texture2DArray gMainTextures : register(t10);

struct PointVertexIn
{
    #ifdef MACRO
        uint Vtable : VTABLE;
    #else
        uint2 Vtable : VTABLE;
    #endif
    
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
