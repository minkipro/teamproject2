#include "BaseRoot.hlsli"

struct PointVertexIn
{
    uint4 RenderInfoIndex : RENDERINFOINDEX;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 Color : TEXCOORD0;
    float2 UV : TEXCOORD1;
    nointerpolation int IsTexVertex : TEXCOORD2;
    //nointerpolation int FixelColId : TEXCOORD3;
};

struct PixelOut
{
    float4  Color : SV_Target0;
    //int     FixelColId : SV_Target1;
};

struct RenderInfo
{               
    float3 PosL;
    float2 Size;
    float4 Color;
    int SpriteInfoIndex;
    int FixelColId;
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

//created for when to use orthographic matrix
void CreatePanel(PointVertexIn vin, inout TriangleStream<VertexOut> output)
{
    VertexOut vertices[4];
    RenderInfo renderInfo = gRenderInfos[vin.RenderInfoIndex.x];
    
    [unroll(4)]
    for (int k = 0; k < 4; k++)
    {
        vertices[k].Color = renderInfo.Color;
        vertices[k].PosH = float4(renderInfo.PosL, 1);
        vertices[k].IsTexVertex = renderInfo.SpriteInfoIndex;
        //vertices[k].FixelColId = renderInfo.FixelColId;
    }
    
    /*
    1 �� 3
    |    |
    0 �� 2
    */
    
    if (renderInfo.SpriteInfoIndex > -1)
    {
        SpriteInfo spInfo = gSpriteInfos[renderInfo.SpriteInfoIndex];
          
        vertices[0].UV = float2(spInfo.StartUV.x, spInfo.EndUV.y);
        vertices[1].UV = spInfo.StartUV;
        vertices[2].UV = spInfo.EndUV;
        vertices[3].UV = float2(spInfo.EndUV.x, spInfo.StartUV.y);
    }
    else
    {
        vertices[0].UV = float2(0, 1.0f);
        vertices[1].UV = float2(0, 0);
        vertices[2].UV = float2(1.0f, 1.0f);
        vertices[3].UV = float2(1.0f, 0);
    }
    
    vertices[0].PosH.xy += float2(0, renderInfo.Size.y);
    vertices[1].PosH.xy += float2(0, 0);
    vertices[2].PosH.xy += float2(renderInfo.Size.x, renderInfo.Size.y);
    vertices[3].PosH.xy += float2(renderInfo.Size.x, 0);

    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        vertices[i].PosH = mul(vertices[i].PosH, gView);
        vertices[i].PosH = mul(vertices[i].PosH, gOrthoMatrix);
    }
    
    /////////////////////////////////////////////////////////////

    output.Append(vertices[0]);
    output.Append(vertices[1]);
    output.Append(vertices[2]);
    output.Append(vertices[3]);
}

PointVertexIn VS(PointVertexIn input, uint id:SV_InstanceID)
{
    input.RenderInfoIndex.x = id;
	return input;
}

[maxvertexcount(4)]
void GS(point PointVertexIn input[1], inout TriangleStream<VertexOut> output)
{
	CreatePanel(input[0], output);
}

PixelOut PS(VertexOut input)
{
    PixelOut result;
    //result.FixelColId = input.FixelColId;
    
    if (input.IsTexVertex >= 0)
    {
        result.Color = gMainTexture.Sample(gsamPointClamp, input.UV);
    }
    else
    {
        result.Color = input.Color;
    }

    
    return result;
}