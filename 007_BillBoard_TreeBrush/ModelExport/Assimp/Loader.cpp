#include "stdafx.h"
#include "Loader.h"
#include "Type.h"
#include "Utilities/Xml.h"
#include "Utilities/BinaryFile.h"

Loader::Loader()
{
	importer = new Assimp::Importer();
}

Loader::~Loader()
{
	SAFE_DELETE(importer);
}

void Loader::ReadFile(wstring file)
{
	this->file = L"../../_Assets/" + file; //파일경로 붙이기

	scene = importer->ReadFile //ReadFile(경로명, 옵션) - importer로 파일을 읽어서 scene을 만들자,, 우리가 필요한대로 만듬
	(
		String::ToString(this->file), // 경로명
		aiProcess_ConvertToLeftHanded // 왼손좌표계로 바꿔줌
		| aiProcess_Triangulate // 삼각형단위로 바꿔줌
		| aiProcess_GenUVCoords // UV좌표가 없으면 자동으로 만들어줌
		| aiProcess_GenNormals // 노멀벡터가 없으면 자동으로 만들어줌
		| aiProcess_CalcTangentSpace // 탄젠트벡터?
	);
	assert(scene != NULL);
}

void Loader::ExportMaterial(wstring savePath, bool bOverwrite)
{
	savePath = L"../../_Textures/" + savePath + L".material"; // .material 확장자 붙여줌

	ReadMaterial();
	WriteMaterial(savePath, bOverwrite);
}

void Loader::ReadMaterial()
{
	for (UINT i = 0; i < scene->mNumMaterials; i++) // mNumMaterials(Material 갯수)
	{
		aiMaterial* readMaterial = scene->mMaterials[i]; // 원본정보,, (Assimp의 모든자료형은 앞에 ai가붙음)
														 // scene->mMaterials는 이중포인터,, 포인터의 배열
		AsMaterial* material = new AsMaterial();	     // 우리가 저장할 자료형(정보)

		material->Name = readMaterial->GetName().C_Str(); // ai를 As에 넣어줌


		//색상 - 일단 가져오는것만 익히고,, 설명은 나중에 조명할때
		float val; //ai_real = float
		aiColor3D color; // Material의 색상은 r,g,b 3개 (a가 없슴).

		readMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color); // 주변색? 광 ex)햇빛.. 노을
		material->Ambient = D3DXCOLOR(color.r, color.g, color.b, 1.0f); // material(우리꺼)에 넣어줌

		readMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color); // 자기 색갈
		material->Diffuse = D3DXCOLOR(color.r, color.g, color.b, 1.0f);

		readMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color); // 거울?
		readMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, val); //
		material->Specular = D3DXCOLOR(color.r, color.g, color.b, 1.0f);

		readMaterial->Get(AI_MATKEY_SHININESS, val);
		material->Shininess = val;



		aiString file; // texture파일 경로명 저장

		readMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		material->DiffuseFile = file.C_Str();

		readMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		material->SpecularFile = file.C_Str();

		readMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
		material->NormalFile = file.C_Str();


		materials.push_back(material); // 씬에있는 정보를 우리꺼에 저장
	}
}

void Loader::WriteMaterial(wstring savePath, bool bOverwrite)
{
	if (bOverwrite == false) // 덮어쓰지 않겟다고하면
	{
		if (Path::ExistFile(savePath) == true) // 파일이 존재하는지 확인하고
			return;							   // 존재하면 그냥 return으로 넘김
	}

	string folder = String::ToString(Path::GetDirectoryName(savePath)); // 폴더명 가져오기
	string file = String::ToString(Path::GetFileName(savePath)); // 파일명 가져오기(확장자까지)

	Path::CreateFolders(folder); // 위에서 만든 floder(폴더명)를 가지고,, 폴더가 없으면 만들어줌





	// Xml,,   Element는 밑으로, Attribute는 옆으로
	Xml::XMLDocument* document = new Xml::XMLDocument(); // xml은 document(문서)로 부터 시작됨,, 항상 이게있어야됨

	Xml::XMLDeclaration* decl = document->NewDeclaration(); // xml파일을 사용하겟다. 선언해줌
	document->LinkEndChild(decl); // LinkEndChild = 밑에 붙이라는 말임,,  cf)최상단은 document

	Xml::XMLElement* root = document->NewElement("Materials"); //NewElement(이름) = 만들어줌,, root부터 시작하자
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance"); // xml형식을 공식적으로 정의해놓은 사이트
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");		  // 이걸 반드시 해줘야 xml형식으로 읽어짐
	document->LinkEndChild(root);
	//---------- 여까지는 공식이라고 생각하자




	for (AsMaterial* material : materials) // materials에 저장해놓은 material를 가져오자
	{
		Xml::XMLElement* node = document->NewElement("Material"); // "Material"라는 이름으로 node를 만듬 (여기에다가 아래에서 만들 element를 넣을거임)
		root->LinkEndChild(node);								  // root("Materials")에 붙임

		Xml::XMLElement* element = NULL; // element(데이터를 붙일거) 만들어놓고

		element = document->NewElement("Name");
		element->SetText(material->Name.c_str());
		node->LinkEndChild(element); // 만든 element를 node("Material")에다가 붙여줌

		element = document->NewElement("DiffuseFile");
		element->SetText(WriteTexture(material->DiffuseFile, folder).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		element->SetText(WriteTexture(material->SpecularFile, folder).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile");
		element->SetText(WriteTexture(material->NormalFile, folder).c_str());
		node->LinkEndChild(element);

		// 색상
		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->Ambient.r); // Attribute = 옆으로 붙임
		element->SetAttribute("G", material->Ambient.g);
		element->SetAttribute("B", material->Ambient.b);
		element->SetAttribute("A", material->Ambient.a);
		node->LinkEndChild(element);					 // 역시 node에 붙여줌
														 //
		element = document->NewElement("Diffuse");
		element->SetAttribute("R", material->Diffuse.r);
		element->SetAttribute("G", material->Diffuse.g);
		element->SetAttribute("B", material->Diffuse.b);
		element->SetAttribute("A", material->Diffuse.a);
		node->LinkEndChild(element);

		element = document->NewElement("Specular");
		element->SetAttribute("R", material->Specular.r);
		element->SetAttribute("G", material->Specular.g);
		element->SetAttribute("B", material->Specular.b);
		element->SetAttribute("A", material->Specular.a);
		node->LinkEndChild(element);

		element = document->NewElement("Shininess");
		element->SetText(material->Shininess);
		node->LinkEndChild(element);

		SAFE_DELETE(material); //material은 어차피 필요한건 다 xml로 만들었으니 지워줌
	}
	//정리하자면 위에서만든 Xml구조는
	//document -> root("Materials") -> node("Material") -> element(각각 이름)
	//이걸 NewElement와 LinkEndChild를 이용해서 연결해준것.


	document->SaveFile((folder + file).c_str()); //SaveFile = 위에 작업한거 저장

}

string Loader::WriteTexture(string file, string saveFolder) //내장된(압축된) 이미지를 압축을 풀어서 저장해주는 함수
{
	if (file.length() < 1) return ""; // 텍스쳐 파일이 없으면 그냥 return (처리할 필요가없으니)

	string fileName = Path::GetFileName(file); // 입력된 file(경로전체가 넘어옴)을 가지고 이름부분만 짤라서 fileName을 만듬
	const aiTexture* texture = scene->GetEmbeddedTexture(file.c_str()); // 내장된텍스쳐를 넣어줌
																		// aiTexture = Assimp의 텍스쳐정보를 가지고있는 객체
	string savePath = ""; // 경로를 받을 string을 선언해두고,
	if (texture != NULL)  // (texture = 내장된텍스쳐)가 NULL이 아니란것은 내장된 텍스쳐가 있다는말
	{//내장
		savePath = saveFolder + Path::GetFileNameWithoutExtension(file) + ".png"; // GetFileNameWithoutExtension() = 확장자를 뺀 이름만 가져와서,, .png를 붙여서 모든건 다 png로

		if (texture->mHeight < 1) // 높이가 1인 텍스쳐는 이미지(픽셀)정보를 다 가지고 있기 때문에,, 그대로 저장만 해주면됨
		{
			FILE* fp;
			fopen_s(&fp, savePath.c_str(), "wb"); // 파일열기                  포인터, 파일명, "wb" = 바이너리파일을 쓰겠다.
			fwrite(texture->pcData, texture->mWidth, 1, fp); // 파일쓰기       이미지정보, 가로줄크기, 세로줄크기, 포인터
			fclose(fp); // 파일닫기
		}
		else // 자유형식으로 저장된 파일,, 나중에설명
		{
			ID3D11Texture2D* dest;
			D3D11_TEXTURE2D_DESC destDesc;
			ZeroMemory(&destDesc, sizeof(D3D11_TEXTURE2D_DESC));
			destDesc.Width = texture->mWidth;
			destDesc.Height = texture->mHeight;
			destDesc.MipLevels = 1;
			destDesc.ArraySize = 1;
			destDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			destDesc.SampleDesc.Count = 1;
			destDesc.SampleDesc.Quality = 0;

			D3D11_SUBRESOURCE_DATA subResource = { 0 };
			subResource.pSysMem = texture->pcData;


			HRESULT hr;
			hr = D3D::GetDevice()->CreateTexture2D(&destDesc, &subResource, &dest);
			assert(SUCCEEDED(hr));

			D3DX11SaveTextureToFileA(D3D::GetDC(), dest, D3DX11_IFF_PNG, savePath.c_str());
		}
	}
	else
	{//외장 = 그냥 복사만 해주면됨
		string directory = Path::GetDirectoryName(String::ToString(this->file)); // 폴더명 받아오고
		string origin = directory + file;										 // 파일명이랑 결합시키고
		String::Replace(&origin, "\\", "/");									 // 상대경로로 바꿔주고

		if (Path::ExistFile(origin) == false)	// 복사할 원본파일이 없으면,, 파일이 없는걸로 간주
			return "";							// 그냥 리턴

		savePath = saveFolder + fileName;						// 파일이있으면 경로와 이름을 조합해서
		CopyFileA(origin.c_str(), savePath.c_str(), FALSE);		// 파일복사

		String::Replace(&savePath, "../../_Textures/", ""); // 파일명만 남기고 경로("../../_Textures/")는 제거
	}


	return Path::GetFileName(savePath); // 파일명만 리턴
}

void Loader::ExportMesh(wstring savePath, bool bOverwrite) // Mesh를 뽑아낼 함수
{
	savePath = L"../../_Models/" + savePath + L".mesh"; //저장경로(_Models)에 .mesh로 저장

	ReadBoneData(scene->mRootNode, -1, -1); // 재귀로 돌릴거임,, root부터, -1, -1
	WriteMeshData(savePath, bOverwrite); //바이너리파일(이진법파일)을 만들어서 mesh데이터를 넣어줄거임
}

void Loader::ReadBoneData(aiNode * node, int index, int parent) // node(ai)를 불러다가 bone(As)에 넣을거임
{
	AsBone* bone = new AsBone(); // 우리가 저장할 자료형 bone만들어 놓고
	bone->Index = index;
	bone->Parent = parent;
	bone->Name = node->mName.C_Str(); // 이름도 node에서 bone에 저장 (어차피 bone에 있는건 node에 있슴)

	D3DXMATRIX transform(node->mTransformation[0]); // 행우선(born) != 열우선(node),,, 그래서 전치해줘야됨
	D3DXMatrixTranspose(&bone->Transform, &transform); // D3DXMatrixTranspose(저장할거(bone), 전치할거(node)) = 전치시켜주는함수

	D3DXMATRIX temp; // 부모행렬을 넣을 temp
	if (parent == -1)						// parent를 확인해서 -1(루트)일 경우
		D3DXMatrixIdentity(&temp);			// 가상으로만든 부모행렬(temp) 넣어주고
	else									// 아닐경우(부모가 있는경우)
		temp = bones[parent]->Transform;	// 부모를 가져옴 (parent의 월드행렬을 뽑아서 넣어줌)

	bone->Transform = bone->Transform * temp; // 아래 참고
	bones.push_back(bone);

	ReadMeshData(node, index); // 노드에있는 MeshData를 불러올거임

	for (UINT i = 0; i < node->mNumChildren; i++) // 재귀,, 자식수만큼 반복문
	{
		// 자기자신을 호출(자식, 함수를 돌때마다 bones.size()가 올라가니까 인덱스는 bones.size(), 부모는 현재 인덱스)
		ReadBoneData(node->mChildren[i], bones.size(), index);
	}

}
// 참고
// 지오메트리 행렬(자신의 월드행렬) = (자신의 지역행렬) * (부모의 월드행렬)
//	 			   자신의 지역행렬 = (자신의 월드행렬) * (부모 월드행렬의 역행렬) // 애니메이션
//≡ Local = World * (Parent World) - 1

void Loader::ReadMeshData(aiNode * node, int bone) //정점정보(Vertex, Index)를 처리할 함수
{
	if (node->mNumMeshes < 1) return; // node에 Mesh가 없으면 읽을게 없으니 리턴,, ex)대포가 나가는 위치

	AsMesh* asMesh = new AsMesh(); // AsMesh(asMesh)에 aiNode(node)를 저장할거임
	asMesh->Name = node->mName.C_Str(); // 이름으로 연결
	asMesh->BoneIndex = bone; // 번호로 연결,, 좀더 빠르게?

	for (UINT i = 0; i < node->mNumMeshes; i++) // 하나의 bone에 참조되는 mesh가 여러개가 있을수도 있으니까
	{
		// 정점을 저장할거임
		UINT index = node->mMeshes[i];			// mMeshes = scene에있는 Mesh배열의 번호
		aiMesh* mesh = scene->mMeshes[index];	// 위에서 구한 index로 scene에 해당 인덱스의 mesh(정점정보)를 가져옴

		UINT startVertex = asMesh->Vertices.size(); // 점점 중첩이되니까 시작번호를 만들어낼수있슴
		UINT startIndex = asMesh->Indices.size();	// 처음엔 물론 시작번호는 0

		for (UINT m = 0; m < mesh->mNumVertices; m++) // mNumVertices = vertexCount
		{
			Model::ModelVertex vertex; // ModelVertex = VertexTextureNormal

									   // 정점정보를 가져오자
			memcpy(&vertex.Position, &mesh->mVertices[m], sizeof(D3DXVECTOR3)); // mesh(aiMesh)에서정점정보를 뽑아와서 vertex에 복사
			memcpy(&vertex.Uv, &mesh->mTextureCoords[0][m], sizeof(D3DXVECTOR2)); // 속도문제때문에 2차원배열로,, [0]으로 해놓고 처음꺼만 쓴다?
			memcpy(&vertex.Normal, &mesh->mNormals[m], sizeof(D3DXVECTOR3));

			asMesh->Vertices.push_back(vertex); // vertex저장
		}//for(m)

		 // 인덱스
		for (UINT f = 0; f < mesh->mNumFaces; f++) //일반적으로 index를 face라고도 부름 (face는 삼각형의 갯수)
		{
			aiFace& face = mesh->mFaces[f];

			for (UINT k = 0; k < face.mNumIndices; k++)
			{
				asMesh->Indices.push_back(face.mIndices[k]);
				asMesh->Indices.back() += startVertex; // Index를 시작버텍스만큼 밀어줌? 왤까
			}
		}//for(f)



		 //meshPart 만들기
		AsMeshPart* meshPart = new AsMeshPart();

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; //mesh에 material번호로 scene에 material을 가져옴
		meshPart->Name = mesh->mName.C_Str();	// Name
		meshPart->MaterialName = material->GetName().C_Str(); // Material(meshPart에 Material번호도 들어감),, 랜더링할때 이 이름을 매칭시켜서 material을 불러올꺼임
		meshPart->StartVertex = startVertex; // 시작버텍스
		meshPart->StartIndex = startIndex; // 시작 인덱스
		meshPart->VertexCount = mesh->mNumVertices; // 버텍스 갯수
		meshPart->IndexCount = mesh->mNumFaces * mesh->mFaces->mNumIndices; // mNumFaces = 3, mNumIndices = 삼각형의갯수,, 이거 두개를 곱해서 인덱스 갯수를 구함

		asMesh->MeshParts.push_back(meshPart); // meshPart저장
	}

	meshes.push_back(asMesh); // asMesh 저장 (aiNode(node)에서 데이터를 가져온거)
}

void Loader::WriteMeshData(wstring savePath, bool bOverwrite)
{
	if (bOverwrite == false) // 덮어쓰지 않겟다고하면
	{
		if (Path::ExistFile(savePath) == true) // 파일이 존재하는지 확인하고
			return;							   // 존재하면 그냥 return으로 넘김
	}

	Path::CreateFolder(Path::GetDirectoryName(savePath)); // 없으면 만들어줌
														  // 로드해서 파일을 만들거임(Binary파일)
	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath); // 파일열기

	w->UInt(bones.size()); //bone이 몇개인지 읽고
	for (AsBone* bone : bones) //반복문돌림
	{
		//struct AsBone 구조에 맞춰서 만들어줌
		w->Int(bone->Index);
		w->String(bone->Name);
		w->Int(bone->Parent);
		w->Matrix(bone->Transform);

		SAFE_DELETE(bone); // bone다썻으면 지우고, 다시 bones 반복문 돌림
	}

	// 같은파일에다가 AsMesh정보도 입력
	w->UInt(meshes.size()); //mesh가 몇개인지 읽고
	for (AsMesh* meshData : meshes) // 반복문 돌림
	{
		w->String(meshData->Name);
		w->Int(meshData->BoneIndex);

		w->UInt(meshData->Vertices.size());
		w->Byte(&meshData->Vertices[0], sizeof(Model::ModelVertex) * meshData->Vertices.size()); // 바이트단위로 벡터크기만큼 벡터를 씀,, ?

		w->UInt(meshData->Indices.size());
		w->Byte(&meshData->Indices[0], sizeof(UINT) * meshData->Indices.size());


		w->UInt(meshData->MeshParts.size());
		for (AsMeshPart* part : meshData->MeshParts)
		{
			w->String(part->Name);
			w->String(part->MaterialName);

			w->UInt(part->StartVertex);
			w->UInt(part->VertexCount);

			w->UInt(part->StartIndex);
			w->UInt(part->IndexCount);

			SAFE_DELETE(part);
		}

		SAFE_DELETE(meshData);
	}

	w->Close(); // 닫기
	SAFE_DELETE(w); //
}
// 정리하자면
// node를 bones에 넣고
// MeshParts를 포함해서, 그리기위한 버텍스,인덱스등등 정보를 뽑아 meshes에 넣고
// bones과 meshes를 바이너리 파일로 만들어서 관리