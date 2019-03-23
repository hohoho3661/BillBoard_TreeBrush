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

Texture2D Map;
Texture2D Map2;

SamplerState Sampler;


float4 PS(VertexOutput input) : SV_TARGET0
{
    //float2 uv = input.Uv - 0.5f; // 쉐이더만 가능한 문법,, 전체가 동일하게 - 됨
    
    float4 color = 0;

    [branch]
    if (input.Uv.x < Uv.x)
        color = Map2.Sample(Sampler, input.Uv);
    else
        color = Map.Sample(Sampler, input.Uv);

    return color;
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