#pragma once
#define MODEL_MAX_TRANSFORMS 128 // 모델이 가질수 있는 최대 bone의 갯수

class ModelBone;
class ModelMesh;


class Model
{
public:
	typedef VertexTextureNormal ModelVertex;

public:
	Model();
	~Model();

	UINT MaterialCount() { return materials.size(); } // 외부에서 사용할함수들
	vector<Material *>& Materials() { return materials; }
	Material* MaterialByIndex(UINT index) { return materials[index]; }
	Material* MaterialByName(wstring name); // 이름같은거 리턴

	UINT BoneCount() { return bones.size(); }
	vector<ModelBone *>& Bones() { return bones; }
	ModelBone* BoneByIndex(UINT index) { return bones[index]; }
	ModelBone* BoneByName(wstring name);

	UINT MeshCount() { return meshes.size(); }
	vector<ModelMesh *>& Meshes() { return meshes; }
	ModelMesh* MeshByIndex(UINT index) { return meshes[index]; }
	ModelMesh* MeshByName(wstring name);


	void ReadMaterial(wstring file); // Xml파일로 저장된 파일을 읽을 함수
	void ReadMesh(wstring file);

private:
	void BindingBone();
	void BindingMesh();

private:
	vector<Material*> materials; // Xml파일을 읽어서 여기에 저장할거임

	ModelBone* root;
	vector<ModelBone *> bones;
	vector<ModelMesh *> meshes;

};