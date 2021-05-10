#include "BaseRoot.hlsli"

struct PointVertexUVIn
{
    float3 PosL : POSITION;
    float2 Size : RENDERSIZE;
    float4 Color : TEXCOORD;
    float4 UV : UV;
    int TexIndex : TEXINDEX;
};

void CreatePanel(PointVertexUVIn vin, inout TriangleStream<VertexOut> output)
{
    VertexOut vertices[4];

    [unroll(4)]
    for (int k = 0; k < 4; k++)
    {
        vertices[k].TexIndex = vin.TexIndex;
        vertices[k].Color = vin.Color;
        vertices[k].PosH = float4(vin.PosL, 1);
    }

    /*
    1 คั 3
    |    |
    0 คั 2
    */
    vertices[0].PosH.xy += float2(0, vin.Size.y);
    vertices[1].PosH.xy += float2(0, 0);
    vertices[2].PosH.xy += float2(vin.Size.x, vin.Size.y);
    vertices[3].PosH.xy += float2(vin.Size.x, 0);

    vertices[0].Color.xy = float2(vin.UV[0], vin.UV[3]);
    vertices[1].Color.xy = float2(vin.UV[0], vin.UV[1]);
    vertices[2].Color.xy = float2(vin.UV[2], vin.UV[3]);
    vertices[3].Color.xy = float2(vin.UV[2], vin.UV[1]);
    
    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        vertices[i].PosH = mul(vertices[i].PosH, gOrthoMatrix);
    }
    
    /////////////////////////////////////////////////////////////

    output.Append(vertices[0]);
    output.Append(vertices[1]);
    output.Append(vertices[2]);
    output.Append(vertices[3]);
}

PointVertexUVIn VS(PointVertexUVIn input)
{
    return input;
}

[maxvertexcount(4)]
void GS(point PointVertexUVIn input[1], inout TriangleStream<VertexOut> output)
{
    CreatePanel(input[0], output);
}