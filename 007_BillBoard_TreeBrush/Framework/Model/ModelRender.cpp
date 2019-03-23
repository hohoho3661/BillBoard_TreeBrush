#include "Framework.h"
#include "ModelRender.h"
#include "ModelMesh.h"

ModelRender::ModelRender(Shader * shader, Model * model)
	: shader(shader), model(model)
{
	renderer = new Renderer();

	for (Material* material : model->Materials())
		material->SetShader(shader);

	

	// 부모자식관계 설정 - 자신의 로컬계산
	D3DXMATRIX transform;
	D3DXMATRIX parentTransform;

	D3DXMATRIX boneTransforms[MODEL_MAX_TRANSFORMS];
	for (UINT i = 0; i < model->BoneCount(); i++)
	{
		ModelBone* bone = model->BoneByIndex(i);

		int parentIndex = bone->ParentIndex();
		if (parentIndex < 0)
			D3DXMatrixIdentity(&parentTransform);
		else
			parentTransform = boneTransforms[parentIndex];

		D3DXMATRIX matrix = bone->Transform();

		boneTransforms[i] = parentTransform;
		transforms[i] = matrix * boneTransforms[i];
	}

	for (ModelMesh* mesh : model->Meshes())
	{
		mesh->SetShader(shader);
		mesh->Transforms(transforms);
	}

//	ModelMesh* turret = model->MeshByName(L"Turret_geo");
//	turret->Position(3, 0, 0);


}

ModelRender::~ModelRender()
{
	SAFE_DELETE(renderer);
}

void ModelRender::Update()
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Update();
}

void ModelRender::Render()
{
	//공통으로 쓸 월드를 세팅
	for (ModelMesh* mesh : model->Meshes())
	{
		D3DXMATRIX t = renderer->World();
		mesh->World(t);
		mesh->Render();
	}
}

void ModelRender::Position(float x, float y, float z)
{
	renderer->Position(x, y, z);
}

void ModelRender::Position(D3DXVECTOR3 & vec)
{
	renderer->Position(vec);
}

void ModelRender::Position(D3DXVECTOR3 * vec)
{
	return renderer->Position(vec);
}

void ModelRender::Scale(float x, float y, float z)
{
	renderer->Scale(x, y, z);
}

void ModelRender::Scale(D3DXVECTOR3 & vec)
{
	renderer->Scale(vec);
}

void ModelRender::Scale(D3DXVECTOR3 * vec)
{
	return renderer->Scale(vec);
}

void ModelRender::Rotation(float x, float y, float z)
{
	renderer->Rotation(x, y, z);
}

void ModelRender::Rotation(D3DXVECTOR3 & vec)
{
	renderer->Rotation(vec);
}

void ModelRender::Rotation(D3DXVECTOR3 * vec)
{
	return renderer->Rotation(vec);
}

void ModelRender::RotationDegree(float x, float y, float z)
{
	renderer->RotationDegree(x, y, z);
}

void ModelRender::RotationDegree(D3DXVECTOR3 & vec)
{
	renderer->RotationDegree(vec);
}

void ModelRender::RotationDegree(D3DXVECTOR3 * vec)
{
	renderer->RotationDegree(vec);
}

void ModelRender::Direction(D3DXVECTOR3 * vec)
{
	renderer->Direction(vec);
}

void ModelRender::Up(D3DXVECTOR3 * vec)
{
	renderer->Up(vec);
}

void ModelRender::Right(D3DXVECTOR3 * vec)
{
	renderer->Right(vec);
}