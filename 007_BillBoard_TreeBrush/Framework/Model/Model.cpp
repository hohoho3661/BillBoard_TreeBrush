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
	for (Material* material : materials) // materials�� �˻��ؼ�
	{
		if (material->Name() == name) // �̸��� ������
			return material;		  // ����
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
	file = L"../../_Textures/" + file + L".material"; // �����̸��� ��ζ� .material�ٰ�

	Xml::XMLDocument* document = new Xml::XMLDocument(); // document�����
	Xml::XMLError error = document->LoadFile(String::ToString(file).c_str()); // ���Ϻҷ��� ����üũ,, ���������� �ҷ�������
	assert(error == Xml::XML_SUCCESS);	// XML_SUCCESS = 0

										//xml�� �ִ� element�ҷ�����
	Xml::XMLElement* root = document->FirstChildElement();	   // root�� document�� ù��°��,, (Materials)
	Xml::XMLElement* materialNode = root->FirstChildElement(); // materialNode�� root�� ù��°�� (Material)

	do
	{
		Material* material = new Material(); // �츮�� ������ material�� ����� ����

		Xml::XMLElement* node = NULL; // ���� �޾ƿ� node�� ����� ����

		node = materialNode->FirstChildElement(); // ù��°��(Name)�� �о
		material->Name(String::ToWString(node->GetText())); //material->Name()�� �־���

															//
		wstring directory = Path::GetDirectoryName(file); // ��θ��� �о directory�� ����
		String::Replace(&directory, L"../../_Textures", L""); // directory���� ��θ� ��(���� ������ ��η� �����Ϸ���)

		wstring texture = L""; // texture�� �ؽ��� ���ϸ� �����Ұ���

		node = node->NextSiblingElement(); // ���������� �Ѿ
		texture = String::ToWString(node->GetText()); // �о texture�� ���� (name�������ϱ� diffusefile)
		if (texture.length() > 0)						//���ڿ��� ���� ���������� 
			material->DiffuseMap(directory + texture);	//�ؽ��ĸ� �������(MaterialŬ���� �ȿ� DiffuseMap()����)

		node = node->NextSiblingElement();
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)
			material->SpecularMap(directory + texture);

		node = node->NextSiblingElement();
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)
			material->NormalMap(directory + texture);

		//����
		D3DXCOLOR color; // color���� ���� ���� ����
		node = node->NextSiblingElement(); // �������� �Ѱ��ְ�
		color.r = node->FloatAttribute("R"); // ���� �о�ͼ� color�� �־���
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->Ambient(color); //material�� color�� �־���

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

		materials.push_back(material); //material�� materials�� �־���,,

		materialNode = materialNode->NextSiblingElement(); //materialNode�� ���� ������ �ҷ�����,, ���ʴ�� �� �ؿ���
	} while (materialNode != NULL); // materialNode�� NULL�� �ƴҶ����� ���� �ݺ���
}

void Model::ReadMesh(wstring file)
{
	file = L"../../_Models/" + file + L".mesh";
	// ������ �о�ð���
	BinaryReader* r = new BinaryReader();
	r->Open(file);

	UINT count = 0;

	count = r->UInt(); // bone�����ŭ �־�аŸ� �ҷ��ͼ� �ݺ��� ������

	for (UINT i = 0; i < count; i++)
	{
		ModelBone* bone = new ModelBone();
		//AsBone���� ���缭
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

			void* ptr = (void *)&(vertices[0]);		// ������ Ȯ���س��� �����Ѱ�
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

// FirstChildElement = ����
// NextSiblingElement = ģ��(���� ���?���� ������)