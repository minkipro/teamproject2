#include "BaseRoot.hlsli"

//created for when to use orthographic matrix
void CreatePanel(PointVertexIn vin, inout TriangleStream<VertexOut> output)
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
    vertices[0].PosH.xy -= vin.Size / 2;
    vertices[1].PosH.xy += float2(-vin.Size.x, vin.Size.y) / 2;
    vertices[2].PosH.xy += float2(vin.Size.x, -vin.Size.y) / 2;
    vertices[3].PosH.xy += vin.Size / 2;

    /////////////////////////////////////////////////////////////

    output.Append(vertices[0]);
    output.Append(vertices[1]);
    output.Append(vertices[2]);
    output.Append(vertices[3]);
}

PointVertexIn VS(PointVertexIn input)
{
	return input;
}

[maxvertexcount(6)]
void GS(point PointVertexIn input[1], inout TriangleStream<VertexOut> output)
{
	CreatePanel(input[0], output);
}

float4 PS(VertexOut input) : SV_TARGET
{
	float4 result = {0,0,0,1};
    
    if(!input.TexIndex < 0)
    {
        result = gMainTextures.Sample(gsamPointWrap, float3(input.TexIndex, input.Color.xy));
    }
    else
    {
        result = input.Color;
    }
	
    return result;
}