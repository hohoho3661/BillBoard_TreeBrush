#include "Framework.h"
#include "Model/ModelMesh.h"
#include "ModelMeshPart.h"

ModelBone::ModelBone()
{
}

ModelBone::~ModelBone()
{
}

///////////////////////////////////////////////////////////////////////////////

ModelMesh::ModelMesh()
{
	boneBuffer = new CBuffer(&boneDesc, sizeof(BoneDesc));
}

ModelMesh::~ModelMesh()
{
	SAFE_DELETE(boneBuffer);

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	for (ModelMeshPart* part : meshParts)
		SAFE_DELETE(part);
}

void ModelMesh::SetShader(Shader * shader)
{
	Renderer::SetShader(shader);

	sBoneBuffer = shader->AsConstantBuffer("CB_Bone");
}

void ModelMesh::Render()
{
	Renderer::Render();

	boneDesc.Index = boneIndex;

	boneBuffer->Apply();
	sBoneBuffer->SetConstantBuffer(boneBuffer->Buffer());

	for (ModelMeshPart* part : meshParts)
		part->Render();
}

void ModelMesh::Transforms(D3DXMATRIX * transforms)
{
	memcpy(boneDesc.Transforms, transforms, sizeof(D3DXMATRIX) * MODEL_MAX_TRANSFORMS);
}

void ModelMesh::Binding()
{
	CreateVertexBuffer(vertices, vertexCount, sizeof(Model::ModelVertex), 0);
	CreateIndexBuffer(indices, indexCount);
}
