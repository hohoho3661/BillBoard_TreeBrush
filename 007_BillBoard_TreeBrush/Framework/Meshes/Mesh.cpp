#include "Framework.h"
#include "Mesh.h"

Mesh::Mesh(Shader * shader)
	: Renderer(shader), color(0,0,0,1)
{
	sColor = shader->AsVector("Color");
}

Mesh::~Mesh()
{
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);
}

void Mesh::Render()
{
	if (vertexBuffer == NULL && indexBuffer == NULL)
	{
		Create();
		CreateVertexBuffer(vertices, vertexCount, sizeof(MeshVertex), 0);
		CreateIndexBuffer(indices, indexCount);
	}

	Renderer::Render();
	sColor->SetFloatVector(color);

	shader->DrawIndexed(0, pass, indexCount);
}

// ÄÃ·¯
void Mesh::Color(D3DXCOLOR & val)
{
	color = val;
}

void Mesh::Color(float r, float g, float b)
{
	Color(D3DXCOLOR(r, g, b, 1));
}