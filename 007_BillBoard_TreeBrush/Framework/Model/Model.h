#pragma once
#define MODEL_MAX_TRANSFORMS 128 // ���� ������ �ִ� �ִ� bone�� ����

class ModelBone;
class ModelMesh;


class Model
{
public:
	typedef VertexTextureNormal ModelVertex;

public:
	Model();
	~Model();

	UINT MaterialCount() { return materials.size(); } // �ܺο��� ������Լ���
	vector<Material *>& Materials() { return materials; }
	Material* MaterialByIndex(UINT index) { return materials[index]; }
	Material* MaterialByName(wstring name); // �̸������� ����

	UINT BoneCount() { return bones.size(); }
	vector<ModelBone *>& Bones() { return bones; }
	ModelBone* BoneByIndex(UINT index) { return bones[index]; }
	ModelBone* BoneByName(wstring name);

	UINT MeshCount() { return meshes.size(); }
	vector<ModelMesh *>& Meshes() { return meshes; }
	ModelMesh* MeshByIndex(UINT index) { return meshes[index]; }
	ModelMesh* MeshByName(wstring name);


	void ReadMaterial(wstring file); // Xml���Ϸ� ����� ������ ���� �Լ�
	void ReadMesh(wstring file);

private:
	void BindingBone();
	void BindingMesh();

private:
	vector<Material*> materials; // Xml������ �о ���⿡ �����Ұ���

	ModelBone* root;
	vector<ModelBone *> bones;
	vector<ModelMesh *> meshes;

};