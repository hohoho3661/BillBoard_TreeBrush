Texture2D BaseMap;
SamplerState BaseSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
};

Texture2D LayerMap;
SamplerState LayerSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
};

Texture2D AlphaMap;
SamplerState AlphaSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
};

float4 GetTerrainColor(float2 uv)
{    
    float4 base = BaseMap.Sample(BaseSampler, uv);
    float4 layer = LayerMap.Sample(LayerSampler, uv);
    float4 alpha = AlphaMap.Sample(AlphaSampler, uv);

    return lerp(base, layer, alpha.r);
}


//-----------------------------------------------------------------------------
// Brush
//-----------------------------------------------------------------------------
cbuffer CB_Brush // C���۴� 16��Ʈ ���ߴ°� ��������
{
    float4 BrushColor;
    float3 BrushLocation;
    int BrushType;
    int BrushRange;
};

float3 GetBrushColor(float3 position)
{
    [flatten]
    if (BrushType == 0)
        return float3(0, 0, 0); // �귯��Ÿ���� 0�̸� �ƹ��͵� ������������

    if (BrushType == 1
        || BrushType == 3) // �簢���������
    {
        if ((position.x >= (BrushLocation.x - BrushRange)) && // �簢�� ���� �Ǵ�
            (position.x <= (BrushLocation.x + BrushRange)) &&
            (position.z >= (BrushLocation.z - BrushRange)) &&
            (position.z <= (BrushLocation.z + BrushRange)))
        {
            return BrushColor.rgb;
        }
    }

    if (BrushType == 2) // �������Ұ���
    {
        float dx = position.x - BrushLocation.x; // �Ÿ����ϱ�
        float dz = position.z - BrushLocation.z;

        float dist = sqrt(dx * dx + dz * dz); // ��Ÿ��������� �Ÿ����ϱ�
        //float dist = distance(dx, dz);

        if (dist <= BrushRange)
            return BrushColor.rgb;
    }

    return float3(0, 0, 0);
}

//-----------------------------------------------------------------------------
// GridLine
//-----------------------------------------------------------------------------

cbuffer CB_GridLine
{
    float4 GridLineColor;

    int VisibleGridLine;
    float GridLineThickness;
    float GridLineSize;
};

//float3 GetGridLineColor(float3 position)
//{
//    [flatten]
//    if (VisibleGridLine == 0)
//        return float3(0, 0, 0);

//    float2 grid = position.xz / GridLineSize;
//    //grid = frac(grid);
//    grid = abs(frac(grid - 0.5f) - 0.5f); // abs�� ���밪,, ����� �������� ����

//    float thick = GridLineThickness / GridLineSize;

//    [flatten]
//    if (grid.x < thick || grid.y < thick)
//        return GridLineColor.rgb;

//    return float3(0, 0, 0);
//}

float3 GetGridLineColor(float3 position)
{
    [flatten]
    if (VisibleGridLine == 0)
        return float3(0, 0, 0);

    float2 grid = position.xz / GridLineSize;
    //grid = frac(grid);
    float2 range = abs(frac(grid - 0.5f) - 0.5f); // abs�� ���밪
    
    float2 speed = fwidth(grid); // fwidth ��̺�,, ��Ƽ �󸮾��,, �־����°ſ����� ��ʰ�(fwidth)�� ���ؼ� �������
    
    float2 pixel = range / speed;

    float weight = saturate(min(pixel.x, pixel.y) - GridLineThickness); // �ȼ�x,y�߿� ������,, 0 or 0.1~0.9

    return lerp(GridLineColor.rgb, float3(0, 0, 0), weight);

    // lerp(A,B,t)
    // t = ���ϰ���� ������ (0����1)
    // (1 - t) * A + t * B
}