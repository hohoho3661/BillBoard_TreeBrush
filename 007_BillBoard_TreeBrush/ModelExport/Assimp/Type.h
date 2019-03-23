#pragma once
#include "stdafx.h"

struct AsMaterial // Material(재질) 정보
{
	string Name; // 이름

	D3DXCOLOR Ambient;
	D3DXCOLOR Diffuse;
	D3DXCOLOR Specular;
	float Shininess;

	string DiffuseFile; //현재 이것만쓸거임,, 우리가 일반적으로 썻던 texture파일
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

	UINT StartVertex; // 시작번호
	UINT VertexCount; // 몇개 그릴지
	
	UINT StartIndex;
	UINT IndexCount;
};

struct AsMesh
{
	string Name;
	int BoneIndex;

	aiMesh* Mesh; // 스키닝? 애니메이션때 씀

	vector<Model::ModelVertex> Vertices;
	vector<UINT> Indices;

	vector<AsMeshPart *> MeshParts;
};