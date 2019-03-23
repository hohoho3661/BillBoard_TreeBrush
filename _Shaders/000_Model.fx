//-----------------------------------------------------------------------------
// Bones
//-----------------------------------------------------------------------------
#define MODEL_MAX_TRANSFORMS 128

cbuffer CB_Bone
{
    matrix BoneTransforms[MODEL_MAX_TRANSFORMS];

    uint Index;
};

void SetModelWorld(inout matrix world) // inout = ���۷���,,, ����,, �ޱ⵵�ϰ� ���ϵ� �Ұ���
{
    world = mul(BoneTransforms[Index], world);
}