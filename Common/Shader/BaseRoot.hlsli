#pragma pack_matrix( row_major )

struct TextureInfo
{
    float2 StartUV;
    float2 EndUV;
    uint   Pad0;
    uint   Pad1;
    uint   Pad2;
    uint   Pad3;
};

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6);

StructuredBuffer<TextureInfo> TextureInfos : register(t20);

float2 GetRealUV(int textureIndexPlusSpriteIndex, float2 uv)
{
    TextureInfo currTextureInfo = TextureInfos[textureIndexPlusSpriteIndex];
    float2 targetUV = currTextureInfo.StartUV;
    float2 scale = currTextureInfo.EndUV - currTextureInfo.StartUV;
    targetUV = targetUV + (uv * scale);
    
    return targetUV;
}