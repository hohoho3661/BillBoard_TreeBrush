#include "Framework.h"
#include "MeshQuad.h"

MeshQuad::MeshQuad(Shader * shader)
	: Mesh(shader)
{
}


MeshQuad::~MeshQuad()
{
}

void MeshQuad::Create()
{
	float w = 0.5f;
	float h = 0.5f;

	vector<MeshVertex> v;
	v.push_back(MeshVertex(-w, -h, 0, 0, 1, 0, 0, -1));
	v.push_back(MeshVertex(-w, +h, 0, 0, 0, 0, 0, -1));
	v.push_back(MeshVertex(+w, -h, 0, 1, 1, 0, 0, -1));
	v.push_back(MeshVertex(+w, +h, 0, 1, 0, 0, 0, -1));

	vertices = new MeshVertex[v.size()];
	vertexCount = v.size();

	copy
	(
		v.begin(), v.end(),
		stdext::checked_array_iterator<MeshVertex *>(vertices, vertexCount) //백터를 배열로 가져올때 (복사해주는)
	);

	// 인덱스
	indexCount = 6;
	indices = new UINT[indexCount]
	{
		0, 1, 2, 2, 1, 3
	};
}