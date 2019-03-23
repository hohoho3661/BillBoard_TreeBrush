#include "000_Header.fx"

//-----------------------------------------------------------------------------
struct VertexOutput
{
    float4 Position : SV_Position0;
    float3 Normal : Normal0;
    float2 Uv : UV0;
};

VertexOutput VS(VertexTextureNormal input)
{
    VertexOutput output;
        
    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);
    output.Normal = WorldNormal(input.Normal); // 월드공간 변화를 무조건 하고난후 Normal 해야됨
    
    output.Uv = input.Uv;

    return output;
}

float4 PS(VertexOutput input) : SV_TARGET0
{
    float4 diffuse = DiffuseMap.Sample(Sampler, input.Uv);        
    
    clip(diffuse.a - 0.3f); // clip 0 이하는 픽셀을 안그리고 폐기시킴

    return diffuse;
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

RasterizerState RS
{
    FillMode = Wireframe;
};

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetRasterizerState(RS);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}