#pragma once
class ModelBone
{
public:
	friend class Model;

public:
	ModelBone();
	~ModelBone();

	int ParentIndex() { return parentIndex; }
	ModelBone* Parent() { return parent; }

	wstring Name() { return name; }

	D3DXMATRIX& Transform() { return transform; }
	void Transform(D3DXMATRIX& matrix) { transform = matrix; }

private:
	int index;
	wstring name;

	int parentIndex;
	ModelBone* parent;

	D3DXMATRIX transform;
	vector<ModelBone *> childs;
};

///////////////////////////////////////////////////////////////////////////////

class ModelMesh : public Renderer
{
public:
	friend class Model;

public:
	ModelMesh();
	~ModelMesh();

	void Binding();

public:
	void SetShader(Shader* shader) override;

	void Render();

	wstring Name() { return name; }

	int BoneIndex() { return boneIndex; }
	class ModelBone* Bone() { return bone; }

	void Transforms(D3DXMATRIX* transforms);


private:
	struct BoneDesc
	{
		D3DXMATRIX Transforms[MODEL_MAX_TRANSFORMS];

		UINT Index;
		float Padding[3];
	} boneDesc;

private:
	wstring name;

	int boneIndex;
	class ModelBone* bone;

	vector<class ModelMeshPart *> meshParts;


	UINT vertexCount;
	Model::ModelVertex* vertices;

	UINT indexCount;
	UINT* indices;

	CBuffer* boneBuffer;
	ID3DX11EffectConstantBuffer* sBoneBuffer;

};