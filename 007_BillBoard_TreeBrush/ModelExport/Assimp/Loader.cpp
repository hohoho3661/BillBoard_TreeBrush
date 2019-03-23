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
	this->file = L"../../_Assets/" + file; //���ϰ�� ���̱�

	scene = importer->ReadFile //ReadFile(��θ�, �ɼ�) - importer�� ������ �о scene�� ������,, �츮�� �ʿ��Ѵ�� ����
	(
		String::ToString(this->file), // ��θ�
		aiProcess_ConvertToLeftHanded // �޼���ǥ��� �ٲ���
		| aiProcess_Triangulate // �ﰢ�������� �ٲ���
		| aiProcess_GenUVCoords // UV��ǥ�� ������ �ڵ����� �������
		| aiProcess_GenNormals // ��ֺ��Ͱ� ������ �ڵ����� �������
		| aiProcess_CalcTangentSpace // ź��Ʈ����?
	);
	assert(scene != NULL);
}

void Loader::ExportMaterial(wstring savePath, bool bOverwrite)
{
	savePath = L"../../_Textures/" + savePath + L".material"; // .material Ȯ���� �ٿ���

	ReadMaterial();
	WriteMaterial(savePath, bOverwrite);
}

void Loader::ReadMaterial()
{
	for (UINT i = 0; i < scene->mNumMaterials; i++) // mNumMaterials(Material ����)
	{
		aiMaterial* readMaterial = scene->mMaterials[i]; // ��������,, (Assimp�� ����ڷ����� �տ� ai������)
														 // scene->mMaterials�� ����������,, �������� �迭
		AsMaterial* material = new AsMaterial();	     // �츮�� ������ �ڷ���(����)

		material->Name = readMaterial->GetName().C_Str(); // ai�� As�� �־���


		//���� - �ϴ� �������°͸� ������,, ������ ���߿� �����Ҷ�
		float val; //ai_real = float
		aiColor3D color; // Material�� ������ r,g,b 3�� (a�� ����).

		readMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color); // �ֺ���? �� ex)�޺�.. ����
		material->Ambient = D3DXCOLOR(color.r, color.g, color.b, 1.0f); // material(�츮��)�� �־���

		readMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color); // �ڱ� ����
		material->Diffuse = D3DXCOLOR(color.r, color.g, color.b, 1.0f);

		readMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color); // �ſ�?
		readMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, val); //
		material->Specular = D3DXCOLOR(color.r, color.g, color.b, 1.0f);

		readMaterial->Get(AI_MATKEY_SHININESS, val);
		material->Shininess = val;



		aiString file; // texture���� ��θ� ����

		readMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		material->DiffuseFile = file.C_Str();

		readMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		material->SpecularFile = file.C_Str();

		readMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
		material->NormalFile = file.C_Str();


		materials.push_back(material); // �����ִ� ������ �츮���� ����
	}
}

void Loader::WriteMaterial(wstring savePath, bool bOverwrite)
{
	if (bOverwrite == false) // ����� �ʰٴٰ��ϸ�
	{
		if (Path::ExistFile(savePath) == true) // ������ �����ϴ��� Ȯ���ϰ�
			return;							   // �����ϸ� �׳� return���� �ѱ�
	}

	string folder = String::ToString(Path::GetDirectoryName(savePath)); // ������ ��������
	string file = String::ToString(Path::GetFileName(savePath)); // ���ϸ� ��������(Ȯ���ڱ���)

	Path::CreateFolders(folder); // ������ ���� floder(������)�� ������,, ������ ������ �������





	// Xml,,   Element�� ������, Attribute�� ������
	Xml::XMLDocument* document = new Xml::XMLDocument(); // xml�� document(����)�� ���� ���۵�,, �׻� �̰��־�ߵ�

	Xml::XMLDeclaration* decl = document->NewDeclaration(); // xml������ ����ϰٴ�. ��������
	document->LinkEndChild(decl); // LinkEndChild = �ؿ� ���̶�� ����,,  cf)�ֻ���� document

	Xml::XMLElement* root = document->NewElement("Materials"); //NewElement(�̸�) = �������,, root���� ��������
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance"); // xml������ ���������� �����س��� ����Ʈ
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");		  // �̰� �ݵ�� ����� xml�������� �о���
	document->LinkEndChild(root);
	//---------- �������� �����̶�� ��������




	for (AsMaterial* material : materials) // materials�� �����س��� material�� ��������
	{
		Xml::XMLElement* node = document->NewElement("Material"); // "Material"��� �̸����� node�� ���� (���⿡�ٰ� �Ʒ����� ���� element�� ��������)
		root->LinkEndChild(node);								  // root("Materials")�� ����

		Xml::XMLElement* element = NULL; // element(�����͸� ���ϰ�) ��������

		element = document->NewElement("Name");
		element->SetText(material->Name.c_str());
		node->LinkEndChild(element); // ���� element�� node("Material")���ٰ� �ٿ���

		element = document->NewElement("DiffuseFile");
		element->SetText(WriteTexture(material->DiffuseFile, folder).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		element->SetText(WriteTexture(material->SpecularFile, folder).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile");
		element->SetText(WriteTexture(material->NormalFile, folder).c_str());
		node->LinkEndChild(element);

		// ����
		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->Ambient.r); // Attribute = ������ ����
		element->SetAttribute("G", material->Ambient.g);
		element->SetAttribute("B", material->Ambient.b);
		element->SetAttribute("A", material->Ambient.a);
		node->LinkEndChild(element);					 // ���� node�� �ٿ���
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

		SAFE_DELETE(material); //material�� ������ �ʿ��Ѱ� �� xml�� ��������� ������
	}
	//�������ڸ� ���������� Xml������
	//document -> root("Materials") -> node("Material") -> element(���� �̸�)
	//�̰� NewElement�� LinkEndChild�� �̿��ؼ� �������ذ�.


	document->SaveFile((folder + file).c_str()); //SaveFile = ���� �۾��Ѱ� ����

}

string Loader::WriteTexture(string file, string saveFolder) //�����(�����) �̹����� ������ Ǯ� �������ִ� �Լ�
{
	if (file.length() < 1) return ""; // �ؽ��� ������ ������ �׳� return (ó���� �ʿ䰡������)

	string fileName = Path::GetFileName(file); // �Էµ� file(�����ü�� �Ѿ��)�� ������ �̸��κи� ©�� fileName�� ����
	const aiTexture* texture = scene->GetEmbeddedTexture(file.c_str()); // ������ؽ��ĸ� �־���
																		// aiTexture = Assimp�� �ؽ��������� �������ִ� ��ü
	string savePath = ""; // ��θ� ���� string�� �����صΰ�,
	if (texture != NULL)  // (texture = ������ؽ���)�� NULL�� �ƴ϶����� ����� �ؽ��İ� �ִٴ¸�
	{//����
		savePath = saveFolder + Path::GetFileNameWithoutExtension(file) + ".png"; // GetFileNameWithoutExtension() = Ȯ���ڸ� �� �̸��� �����ͼ�,, .png�� �ٿ��� ���� �� png��

		if (texture->mHeight < 1) // ���̰� 1�� �ؽ��Ĵ� �̹���(�ȼ�)������ �� ������ �ֱ� ������,, �״�� ���常 ���ָ��
		{
			FILE* fp;
			fopen_s(&fp, savePath.c_str(), "wb"); // ���Ͽ���                  ������, ���ϸ�, "wb" = ���̳ʸ������� ���ڴ�.
			fwrite(texture->pcData, texture->mWidth, 1, fp); // ���Ͼ���       �̹�������, ������ũ��, ������ũ��, ������
			fclose(fp); // ���ϴݱ�
		}
		else // ������������ ����� ����,, ���߿�����
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
	{//���� = �׳� ���縸 ���ָ��
		string directory = Path::GetDirectoryName(String::ToString(this->file)); // ������ �޾ƿ���
		string origin = directory + file;										 // ���ϸ��̶� ���ս�Ű��
		String::Replace(&origin, "\\", "/");									 // ����η� �ٲ��ְ�

		if (Path::ExistFile(origin) == false)	// ������ ���������� ������,, ������ ���°ɷ� ����
			return "";							// �׳� ����

		savePath = saveFolder + fileName;						// ������������ ��ο� �̸��� �����ؼ�
		CopyFileA(origin.c_str(), savePath.c_str(), FALSE);		// ���Ϻ���

		String::Replace(&savePath, "../../_Textures/", ""); // ���ϸ� ����� ���("../../_Textures/")�� ����
	}


	return Path::GetFileName(savePath); // ���ϸ� ����
}

void Loader::ExportMesh(wstring savePath, bool bOverwrite) // Mesh�� �̾Ƴ� �Լ�
{
	savePath = L"../../_Models/" + savePath + L".mesh"; //������(_Models)�� .mesh�� ����

	ReadBoneData(scene->mRootNode, -1, -1); // ��ͷ� ��������,, root����, -1, -1
	WriteMeshData(savePath, bOverwrite); //���̳ʸ�����(����������)�� ���� mesh�����͸� �־��ٰ���
}

void Loader::ReadBoneData(aiNode * node, int index, int parent) // node(ai)�� �ҷ��ٰ� bone(As)�� ��������
{
	AsBone* bone = new AsBone(); // �츮�� ������ �ڷ��� bone����� ����
	bone->Index = index;
	bone->Parent = parent;
	bone->Name = node->mName.C_Str(); // �̸��� node���� bone�� ���� (������ bone�� �ִ°� node�� �ֽ�)

	D3DXMATRIX transform(node->mTransformation[0]); // ��켱(born) != ���켱(node),,, �׷��� ��ġ����ߵ�
	D3DXMatrixTranspose(&bone->Transform, &transform); // D3DXMatrixTranspose(�����Ұ�(bone), ��ġ�Ұ�(node)) = ��ġ�����ִ��Լ�

	D3DXMATRIX temp; // �θ������ ���� temp
	if (parent == -1)						// parent�� Ȯ���ؼ� -1(��Ʈ)�� ���
		D3DXMatrixIdentity(&temp);			// �������θ��� �θ����(temp) �־��ְ�
	else									// �ƴҰ��(�θ� �ִ°��)
		temp = bones[parent]->Transform;	// �θ� ������ (parent�� ��������� �̾Ƽ� �־���)

	bone->Transform = bone->Transform * temp; // �Ʒ� ����
	bones.push_back(bone);

	ReadMeshData(node, index); // ��忡�ִ� MeshData�� �ҷ��ð���

	for (UINT i = 0; i < node->mNumChildren; i++) // ���,, �ڽļ���ŭ �ݺ���
	{
		// �ڱ��ڽ��� ȣ��(�ڽ�, �Լ��� �������� bones.size()�� �ö󰡴ϱ� �ε����� bones.size(), �θ�� ���� �ε���)
		ReadBoneData(node->mChildren[i], bones.size(), index);
	}

}
// ����
// ������Ʈ�� ���(�ڽ��� �������) = (�ڽ��� �������) * (�θ��� �������)
//	 			   �ڽ��� ������� = (�ڽ��� �������) * (�θ� ��������� �����) // �ִϸ��̼�
//�� Local = World * (Parent World) - 1

void Loader::ReadMeshData(aiNode * node, int bone) //��������(Vertex, Index)�� ó���� �Լ�
{
	if (node->mNumMeshes < 1) return; // node�� Mesh�� ������ ������ ������ ����,, ex)������ ������ ��ġ

	AsMesh* asMesh = new AsMesh(); // AsMesh(asMesh)�� aiNode(node)�� �����Ұ���
	asMesh->Name = node->mName.C_Str(); // �̸����� ����
	asMesh->BoneIndex = bone; // ��ȣ�� ����,, ���� ������?

	for (UINT i = 0; i < node->mNumMeshes; i++) // �ϳ��� bone�� �����Ǵ� mesh�� �������� �������� �����ϱ�
	{
		// ������ �����Ұ���
		UINT index = node->mMeshes[i];			// mMeshes = scene���ִ� Mesh�迭�� ��ȣ
		aiMesh* mesh = scene->mMeshes[index];	// ������ ���� index�� scene�� �ش� �ε����� mesh(��������)�� ������

		UINT startVertex = asMesh->Vertices.size(); // ���� ��ø�̵Ǵϱ� ���۹�ȣ�� �������ֽ�
		UINT startIndex = asMesh->Indices.size();	// ó���� ���� ���۹�ȣ�� 0

		for (UINT m = 0; m < mesh->mNumVertices; m++) // mNumVertices = vertexCount
		{
			Model::ModelVertex vertex; // ModelVertex = VertexTextureNormal

									   // ���������� ��������
			memcpy(&vertex.Position, &mesh->mVertices[m], sizeof(D3DXVECTOR3)); // mesh(aiMesh)�������������� �̾ƿͼ� vertex�� ����
			memcpy(&vertex.Uv, &mesh->mTextureCoords[0][m], sizeof(D3DXVECTOR2)); // �ӵ����������� 2�����迭��,, [0]���� �س��� ó������ ����?
			memcpy(&vertex.Normal, &mesh->mNormals[m], sizeof(D3DXVECTOR3));

			asMesh->Vertices.push_back(vertex); // vertex����
		}//for(m)

		 // �ε���
		for (UINT f = 0; f < mesh->mNumFaces; f++) //�Ϲ������� index�� face��� �θ� (face�� �ﰢ���� ����)
		{
			aiFace& face = mesh->mFaces[f];

			for (UINT k = 0; k < face.mNumIndices; k++)
			{
				asMesh->Indices.push_back(face.mIndices[k]);
				asMesh->Indices.back() += startVertex; // Index�� ���۹��ؽ���ŭ �о���? �ͱ�
			}
		}//for(f)



		 //meshPart �����
		AsMeshPart* meshPart = new AsMeshPart();

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; //mesh�� material��ȣ�� scene�� material�� ������
		meshPart->Name = mesh->mName.C_Str();	// Name
		meshPart->MaterialName = material->GetName().C_Str(); // Material(meshPart�� Material��ȣ�� ��),, �������Ҷ� �� �̸��� ��Ī���Ѽ� material�� �ҷ��ò���
		meshPart->StartVertex = startVertex; // ���۹��ؽ�
		meshPart->StartIndex = startIndex; // ���� �ε���
		meshPart->VertexCount = mesh->mNumVertices; // ���ؽ� ����
		meshPart->IndexCount = mesh->mNumFaces * mesh->mFaces->mNumIndices; // mNumFaces = 3, mNumIndices = �ﰢ���ǰ���,, �̰� �ΰ��� ���ؼ� �ε��� ������ ����

		asMesh->MeshParts.push_back(meshPart); // meshPart����
	}

	meshes.push_back(asMesh); // asMesh ���� (aiNode(node)���� �����͸� �����°�)
}

void Loader::WriteMeshData(wstring savePath, bool bOverwrite)
{
	if (bOverwrite == false) // ����� �ʰٴٰ��ϸ�
	{
		if (Path::ExistFile(savePath) == true) // ������ �����ϴ��� Ȯ���ϰ�
			return;							   // �����ϸ� �׳� return���� �ѱ�
	}

	Path::CreateFolder(Path::GetDirectoryName(savePath)); // ������ �������
														  // �ε��ؼ� ������ �������(Binary����)
	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath); // ���Ͽ���

	w->UInt(bones.size()); //bone�� ����� �а�
	for (AsBone* bone : bones) //�ݺ�������
	{
		//struct AsBone ������ ���缭 �������
		w->Int(bone->Index);
		w->String(bone->Name);
		w->Int(bone->Parent);
		w->Matrix(bone->Transform);

		SAFE_DELETE(bone); // bone�ٛ����� �����, �ٽ� bones �ݺ��� ����
	}

	// �������Ͽ��ٰ� AsMesh������ �Է�
	w->UInt(meshes.size()); //mesh�� ����� �а�
	for (AsMesh* meshData : meshes) // �ݺ��� ����
	{
		w->String(meshData->Name);
		w->Int(meshData->BoneIndex);

		w->UInt(meshData->Vertices.size());
		w->Byte(&meshData->Vertices[0], sizeof(Model::ModelVertex) * meshData->Vertices.size()); // ����Ʈ������ ����ũ�⸸ŭ ���͸� ��,, ?

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

	w->Close(); // �ݱ�
	SAFE_DELETE(w); //
}
// �������ڸ�
// node�� bones�� �ְ�
// MeshParts�� �����ؼ�, �׸������� ���ؽ�,�ε������ ������ �̾� meshes�� �ְ�
// bones�� meshes�� ���̳ʸ� ���Ϸ� ���� ����