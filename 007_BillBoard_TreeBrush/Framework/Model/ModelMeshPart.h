#pragma once

class ModelMeshPart
{
public:
	friend class Model;
	friend class Models;
	friend class ModelMesh;

private:
	ModelMeshPart();
	~ModelMeshPart();

	void Render();

private:
	wstring name;

	class ModelMesh* parent;

	Material* material;
	wstring materialName;

	UINT startVertex;
	UINT vertexCount;

	UINT startIndex;
	UINT indexCount;
};