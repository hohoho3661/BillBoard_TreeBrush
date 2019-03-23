#include "000_Header.fx"


//-----------------------------------------------------------------------------

struct VertexOutput
{
    float4 Position : SV_Position0;
    float3 Normal : Normal0;
};

VertexOutput VS(VertexNormal input)
{
    VertexOutput output;
    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);

    output.Normal = WorldNormal(input.Normal); // 월드공간 변화를 무조건 하고난후 Normal 해야됨


    return output;
}

float4 PS(VertexOutput input) : SV_TARGET0
{
    float4 diffuse = float4(1, 1, 1, 1);
    
    float3 normal = normalize(input.Normal);
    float3 light = -LightDirection;
    float NdotL = dot(normal, light);

    return diffuse = NdotL;
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

struct VertexInput2
{
    float4 Position : Position0;
    float3 Normal : Normal0;

};

struct VertexOutput2
{
    float4 Position : SV_Position0;
    float4 Color : Color0; // 번호만 중복안되게 쓰고싶은거 쓰면됨
    float3 Normal : Normal0;

};

VertexOutput2 VS2(VertexInput2 input)
{
    VertexOutput2 output;

    output.Color = float4(1, 1, 1, 1);
    if (input.Position.y >5)
        output.Color = float4(1, 0, 0, 1);
    if (input.Position.y > 10)
        output.Color = float4(0, 1, 0, 1);
    if (input.Position.y > 20)
        output.Color = float4(0, 0, 1, 1);

    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);

    output.Normal = WorldNormal(input.Normal); // 월드공간 변화를 무조건 하고난후 Normal 해야됨


    return output;
}

float4 PS2(VertexOutput2 input) : SV_TARGET0
{
    float4 diffuse = input.Color;
    
    float3 normal = normalize(input.Normal);
    float3 light = -LightDirection;
    float NdotL = dot(normal, light);

    return diffuse * NdotL;
}
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
        SetVertexShader(CompileShader(vs_5_0, VS2()));
        SetPixelShader(CompileShader(ps_5_0, PS2()));
    }
}