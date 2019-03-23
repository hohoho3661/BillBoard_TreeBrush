#include "000_Header.fx"

//-----------------------------------------------------------------------------

struct VertexOutput
{
    float4 Position : SV_Position0;
    float2 Uv : Uv0;
};

VertexOutput VS(VertexTexture input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;


    return output;
}


float2 Uv;
uint Address;
uint Filter;

Texture2D Map;
SamplerState SamplerAddress // SamplerState에 쉐이더 옵션이 들어감
{
    AddressU = WRAP;
    AddressV = WRAP;
};
SamplerState SamplerAddress2
{
    AddressU = MIRROR;
    AddressV = MIRROR;
};
SamplerState SamplerAddress3
{
    AddressU = CLAMP;
    AddressV = CLAMP;
};
SamplerState SamplerAddress4
{
    AddressU = BORDER;
    AddressV = BORDER;

    BorderColor = float4(1, 0, 0, 1);
};

//
SamplerState SamplerFilter
{
    Filter = MIN_MAG_MIP_POINT;
};
SamplerState SamplerFilter2
{
    Filter = MIN_MAG_MIP_LINEAR;
};

//float4 PS(VertexOutput input) : SV_TARGET0
//{    
//    //float2 uv = input.Uv - 0.5f; // 쉐이더만 가능한 문법,, 전체가 동일하게 - 됨
    
//    float4 color = 0;

//    [branch]
//    if (input.Uv.x < Uv.x)
//        color = float4(0, 0, 0, 1);
//    else
//        color = Map.Sample(Sampler, input.Uv);

//    return color;
//}

//float4 PS(VertexOutput input) : SV_TARGET0
//{
//    float4 color = 0;

//    [brench]
//    switch (Address)
//    {
//        case 0:
//            color = Map.Sample(SamplerAddress, input.Uv);
//            break;
//        case 1:
//            color = Map.Sample(SamplerAddress2, input.Uv);
//            break;
//        case 2:
//            color = Map.Sample(SamplerAddress3, input.Uv);
//            break;
//        case 3:
//            color = Map.Sample(SamplerAddress4, input.Uv);
//            break;
//    }

//    return color;
//    //return Map.Sample(SamplerAddress, input.Uv);
//}
float4 PS(VertexOutput input) : SV_TARGET0
{
    float4 color = 0;

    [brench]
    switch (Filter)
    {
        case 0:
            color = Map.Sample(SamplerFilter, input.Uv);
            break;
        case 1:
            color = Map.Sample(SamplerFilter2, input.Uv);
            break;
    }

    return color;
    //return Map.Sample(SamplerAddress, input.Uv);
}
//-----------------------------------------------------------------------------

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}