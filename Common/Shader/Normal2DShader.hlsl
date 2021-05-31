#include "BaseRoot.hlsli"

struct VertexIn
{
    float3 Pos : POSITION;
    int Pad : PAD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 Color : TEXCOORD0;
    float2 UV : TEXCOORD1;
    nointerpolation int TextureIndex : TEXINDEX0;
    nointerpolation uint SpriteIndex : TEXINDEX1;
};

struct RenderInfo
{
    float3 PosL;
    float2 Size;
    float4 Color;
    int textureIndex;
    uint spriteIndex;
    int pad1;
};

cbuffer cbPass : register(b0)
{
    float4x4 gView;
    float4x4 gProj;
    float4x4 gViewProj;
    float4x4 gOrthoMatrix;
    float2 gMousePos;
    uint2 gRenderTargetSize;
};

Texture2D gMainTexture : register(t0);
StructuredBuffer<RenderInfo> gRenderInfos : register(t1);

VertexOut VS(VertexIn input, uint id : SV_InstanceID)
{
    VertexOut vertex;
    RenderInfo renderInfo = gRenderInfos[id];
    
    vertex.TextureIndex = renderInfo.textureIndex;
    vertex.SpriteIndex = renderInfo.spriteIndex;
    
    vertex.Color = renderInfo.Color;
    vertex.UV = input.Pos.xy;
    
    vertex.PosH = float4(input.Pos, 1);
    vertex.PosH.xy *= renderInfo.Size;
    vertex.PosH.xyz += renderInfo.PosL;
   
    vertex.PosH = mul(vertex.PosH, gView);
    vertex.PosH = mul(vertex.PosH, gOrthoMatrix);
    
    return vertex;
}

float4 PS(VertexOut input) : SV_TARGET
{
    float4 result = { 0, 0, 0, 1 };
    
    if (input.TextureIndex >= 0)
    {
        result = gMainTexture.Sample(gsamPointClamp, GetSpriteUV(input.SpriteIndex, input.UV));
    }
    else
    {
        result = input.Color;
    }

    return result;
}