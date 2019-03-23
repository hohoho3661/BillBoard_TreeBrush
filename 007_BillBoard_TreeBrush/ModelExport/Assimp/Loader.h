#pragma once
//Type.h���� ���� �ڷ����� �θ��� Ŭ����
//Assimp�κ��� ������ �о �츮�� ���ϴ� ���·� ��ȯ�� ���ٰ���
class Loader
{
public:
	Loader();
	~Loader();

	void ReadFile(wstring file); //�ܺο��� �������ְ�

public://Material
	void ExportMaterial(wstring savePath, bool bOverwrite = true); //Material �̾Ƴ��Լ� (������, �������)

private:
	void ReadMaterial(); // ReadFile()���� ���� scene���κ��� �о�� �Լ�
	void WriteMaterial(wstring savePath, bool bOverwrite);
	string WriteTexture(string file, string saveFolder);

public://Mesh
	void ExportMesh(wstring savePath, bool bOverwrite = true); // Mesh�� �̾Ƴ� �Լ�

private:
	void ReadBoneData(aiNode* node, int index, int parent);//node���� bone�����͸� ������,, Ʈ�� ������ ���ֽ�,, ��͸�����
	void ReadMeshData(aiNode* node, int bone); //mesh�����͸� ������,, ReadBoneData()���� ȣ���Ұ���
	void WriteMeshData(wstring savePath, bool bOverwrite); //mesh������,, �ε��ؼ� ������ �������


private:
	wstring file; // �ҷ��� ����

	Assimp::Importer* importer; // importer���� �ҷ����� �����Ұ���
	const aiScene* scene; // scene�� ��������� ���Ϲ޾ƾߵ�

	vector<struct AsMaterial *> materials; //�츮�� �ʿ��� ������ ���ͼ� �����Ұ���
	vector<struct AsBone *> bones;
	vector<struct AsMesh *> meshes;
};

// ReadFile()���� �츮�� ������ �ɼǴ�� scene�� �����
// ExportMaterial()���� ���������� scene���� �츮�� �ʿ��� ������ �̾Ƴ���