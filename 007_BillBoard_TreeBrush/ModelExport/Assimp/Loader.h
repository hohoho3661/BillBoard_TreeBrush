#pragma once
//Type.h에서 만든 자료형을 부르는 클래스
//Assimp로부터 파일을 읽어서 우리가 원하는 형태로 변환을 해줄거임
class Loader
{
public:
	Loader();
	~Loader();

	void ReadFile(wstring file); //외부에서 읽을수있게

public://Material
	void ExportMaterial(wstring savePath, bool bOverwrite = true); //Material 뽑아낼함수 (저장경로, 덮어쓸건지)

private:
	void ReadMaterial(); // ReadFile()에서 만든 scene으로부터 읽어올 함수
	void WriteMaterial(wstring savePath, bool bOverwrite);
	string WriteTexture(string file, string saveFolder);

public://Mesh
	void ExportMesh(wstring savePath, bool bOverwrite = true); // Mesh를 뽑아낼 함수

private:
	void ReadBoneData(aiNode* node, int index, int parent);//node에서 bone데이터를 가져옴,, 트리 구조로 되있슴,, 재귀를쓰자
	void ReadMeshData(aiNode* node, int bone); //mesh데이터를 가져옴,, ReadBoneData()에서 호출할거임
	void WriteMeshData(wstring savePath, bool bOverwrite); //mesh데이터,, 로드해서 파일을 만들거임


private:
	wstring file; // 불러올 파일

	Assimp::Importer* importer; // importer에서 불러들일 역할할거임
	const aiScene* scene; // scene은 상수형으로 리턴받아야됨

	vector<struct AsMaterial *> materials; //우리가 필요한 정보를 빼와서 저장할거임
	vector<struct AsBone *> bones;
	vector<struct AsMesh *> meshes;
};

// ReadFile()에서 우리가 설정한 옵션대로 scene을 만들고
// ExportMaterial()에서 위에서만든 scene에서 우리가 필요한 정보를 뽑아낸다