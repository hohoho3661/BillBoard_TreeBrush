#include "framework.h"
#include "ModelMeshPart.h"
#include "ModelMesh.h"

ModelMeshPart::ModelMeshPart()
	: material(NULL)
{

}

ModelMeshPart::~ModelMeshPart()
{

}

void ModelMeshPart::Render()
{
	material->Render();

	parent->GetShader()->DrawIndexed(0, parent->Pass(), indexCount, startIndex, 0);
	//material->GetShader()->DrawIndexed(0, parent->Pass(), indexCount, startIndex, 0);
}