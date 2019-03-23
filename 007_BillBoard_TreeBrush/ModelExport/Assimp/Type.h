#pragma once
#include "stdafx.h"

struct AsMaterial // Material(����) ����
{
	string Name; // �̸�

	D3DXCOLOR Ambient;
	D3DXCOLOR Diffuse;
	D3DXCOLOR Specular;
	float Shininess;

	string DiffuseFile; //���� �̰͸�������,, �츮�� �Ϲ������� ���� texture����
	string SpecularFile;
	string NormalFile;
};

struct AsBone
{
	int Index;
	string Name;

	int Parent;
	D3DXMATRIX Transform;
};

struct AsMeshPart
{
	string Name;
	string MaterialName;

	UINT StartVertex; // ���۹�ȣ
	UINT VertexCount; // � �׸���
	
	UINT StartIndex;
	UINT IndexCount;
};

struct AsMesh
{
	string Name;
	int BoneIndex;

	aiMesh* Mesh; // ��Ű��? �ִϸ��̼Ƕ� ��

	vector<Model::ModelVertex> Vertices;
	vector<UINT> Indices;

	vector<AsMeshPart *> MeshParts;
};