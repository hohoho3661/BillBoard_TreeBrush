#include "Framework.h"
#include "Model.h"
#include "ModelMesh.h"
#include "ModelMeshPart.h"
#include "Utilities/Xml.h"
#include "Utilities/BinaryFile.h"

Model::Model()
{
}

Model::~Model()
{
	for (Material* material : materials)
		SAFE_DELETE(material);

	for (ModelBone* bone : bones)
		SAFE_DELETE(bone);

	for (ModelMesh* mesh : meshes)
		SAFE_DELETE(mesh);
}

Material * Model::MaterialByName(wstring name)
{
	for (Material* material : materials) // materials을 검색해서
	{
		if (material->Name() == name) // 이름이 같으면
			return material;		  // 리턴
	}

	return NULL;
}

ModelBone * Model::BoneByName(wstring name)
{
	for (ModelBone* bone : bones)
	{
		if (bone->Name() == name)
			return bone;
	}

	return NULL;
}

ModelMesh * Model::MeshByName(wstring name)
{
	for (ModelMesh* mesh : meshes)
	{
		if (mesh->Name() == name)
			return mesh;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////


void Model::ReadMaterial(wstring file)
{
	file = L"../../_Textures/" + file + L".material"; // 파일이름에 경로랑 .material붙게

	Xml::XMLDocument* document = new Xml::XMLDocument(); // document만들고
	Xml::XMLError error = document->LoadFile(String::ToString(file).c_str()); // 파일불러서 에러체크,, 정상적으로 불러졌는지
	assert(error == Xml::XML_SUCCESS);	// XML_SUCCESS = 0

										//xml에 있는 element불러오기
	Xml::XMLElement* root = document->FirstChildElement();	   // root는 document의 첫번째꺼,, (Materials)
	Xml::XMLElement* materialNode = root->FirstChildElement(); // materialNode는 root의 첫번째꺼 (Material)

	do
	{
		Material* material = new Material(); // 우리꺼 저장할 material를 만들어 놓고

		Xml::XMLElement* node = NULL; // 값을 받아올 node를 만들어 놓고

		node = materialNode->FirstChildElement(); // 첫번째꺼(Name)을 읽어서
		material->Name(String::ToWString(node->GetText())); //material->Name()에 넣어줌

															//
		wstring directory = Path::GetDirectoryName(file); // 경로명을 읽어서 directory에 저장
		String::Replace(&directory, L"../../_Textures", L""); // directory에서 경로를 뺌(새로 지정한 경로로 적용하려고)

		wstring texture = L""; // texture에 텍스쳐 파일명 저장할거임

		node = node->NextSiblingElement(); // 같은형제로 넘어감
		texture = String::ToWString(node->GetText()); // 읽어서 texture에 넣음 (name다음꺼니까 diffusefile)
		if (texture.length() > 0)						//문자열에 값이 들어왔을때만 
			material->DiffuseMap(directory + texture);	//텍스쳐를 만들어줌(Material클래스 안에 DiffuseMap()으로)

		node = node->NextSiblingElement();
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)
			material->SpecularMap(directory + texture);

		node = node->NextSiblingElement();
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)
			material->NormalMap(directory + texture);

		//색상
		D3DXCOLOR color; // color값을 받을 변수 생성
		node = node->NextSiblingElement(); // 다음으로 넘겨주고
		color.r = node->FloatAttribute("R"); // 값을 읽어와서 color에 넣어줌
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->Ambient(color); //material에 color값 넣어줌

								  //
		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->Diffuse(color);

		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->Specular(color);

		node = node->NextSiblingElement();
		material->Shininess(node->FloatText());

		materials.push_back(material); //material를 materials에 넣어줌,,

		materialNode = materialNode->NextSiblingElement(); //materialNode에 대한 형제를 불러들임,, 차례대로 그 밑에꺼
	} while (materialNode != NULL); // materialNode가 NULL이 아닐때까지 위에 반복함
}

void Model::ReadMesh(wstring file)
{
	file = L"../../_Models/" + file + L".mesh";
	// 파일을 읽어올거임
	BinaryReader* r = new BinaryReader();
	r->Open(file);

	UINT count = 0;

	count = r->UInt(); // bone사이즈만큼 넣어둔거를 불러와서 반복문 돌릴거

	for (UINT i = 0; i < count; i++)
	{
		ModelBone* bone = new ModelBone();
		//AsBone형식 맞춰서
		bone->index = r->Int();
		bone->name = String::ToWString(r->String());
		bone->parentIndex = r->Int();
		bone->transform = r->Matrix();

		bones.push_back(bone);
	}

	count = r->UInt();
	for (UINT i = 0; i < count; i++)
	{
		ModelMesh* mesh = new ModelMesh();

		mesh->name = String::ToWString(r->String());
		mesh->boneIndex = r->Int();

		//VertexData
		{
			UINT count = r->UInt();

			vector<Model::ModelVertex> vertices;
			vertices.assign(count, Model::ModelVertex());

			void* ptr = (void *)&(vertices[0]);		// 사이즈 확보해놓고 복사한거
			r->Byte(&ptr, sizeof(Model::ModelVertex) * count);


			mesh->vertices = new Model::ModelVertex[count];
			mesh->vertexCount = count;
			copy
			(
				vertices.begin(), vertices.end(),
				stdext::checked_array_iterator<Model::ModelVertex *>(mesh->vertices, count)
			);
		}

		//IndexData
		{
			UINT count = r->UInt();

			vector<UINT> indices;
			indices.assign(count, UINT());

			void* ptr = (void *)&(indices[0]);
			r->Byte(&ptr, sizeof(UINT) * count);


			mesh->indices = new UINT[count];
			mesh->indexCount = count;
			copy
			(
				indices.begin(), indices.end(),
				stdext::checked_array_iterator<UINT *>(mesh->indices, count)
			);
		}


		UINT partCount = r->UInt();
		for (UINT k = 0; k < partCount; k++)
		{
			ModelMeshPart* meshPart = new ModelMeshPart();
			meshPart->parent = mesh;

			meshPart->name = String::ToWString(r->String());
			meshPart->materialName = String::ToWString(r->String());

			meshPart->startVertex = r->UInt();
			meshPart->vertexCount = r->UInt();

			meshPart->startIndex = r->UInt();
			meshPart->indexCount = r->UInt();

			mesh->meshParts.push_back(meshPart);
		}//for(k)

		meshes.push_back(mesh);
	}//for(i)

	r->Close();
	SAFE_DELETE(r);

	BindingBone();
	BindingMesh();
}

void Model::BindingBone()
{
	root = bones[0];
	for (ModelBone* bone : bones)
	{
		if (bone->parentIndex > -1)
		{
			bone->parent = bones[bone->parentIndex];
			bone->parent->childs.push_back(bone);
		}
		else
			bone->parent = NULL;
	}
}

void Model::BindingMesh()
{
	for (ModelMesh* mesh : meshes)
	{
		for (ModelBone* bone : bones)
		{
			if (mesh->boneIndex == bone->index)
			{
				mesh->bone = bone;

				break;
			}
		}

		for (ModelMeshPart* part : mesh->meshParts)
		{
			for (Material* material : materials)
			{
				if (part->materialName == material->Name())
				{
					part->material = material;

					break;
				}
			}
		}

		mesh->Binding();
	}
}

// FirstChildElement = 하위
// NextSiblingElement = 친구(같은 등급?에서 다음꺼)