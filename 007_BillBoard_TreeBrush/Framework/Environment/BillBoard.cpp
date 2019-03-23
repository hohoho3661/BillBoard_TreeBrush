#include "Framework.h"
#include "BillBoard.h"

BillBoard::BillBoard(Shader * shader, wstring file)
	: shader(shader), fixedY(true)
{
	texture = new Texture(file);

	quad = new MeshQuad(shader);
	shader->AsSRV("DiffuseMap")->SetResource(texture->SRV()); // "DiffuseMap"Àº Texture2D Map
}

BillBoard::~BillBoard()
{
	SAFE_DELETE(texture);
	SAFE_DELETE(quad);

}

void BillBoard::Position(float x, float y, float z)
{
	quad->Position(x, y, z);
}

void BillBoard::Position(D3DXVECTOR3 vec)
{
	quad->Position(vec);
}

void BillBoard::Position(D3DXVECTOR3 * vec)
{
	quad->Position(vec);
}

void BillBoard::Scale(float x, float y, float z)
{
	quad->Scale(x, y, z);
}

void BillBoard::Scale(D3DXVECTOR3 * vec)
{
	quad->Scale(vec);
}

void BillBoard::Scale(D3DXVECTOR3 vec)
{
	quad->Scale(vec);
}

void BillBoard::Update()
{
	//decompose ¹æ½Ä
	D3DXMATRIX V = Context::Get()->View();
	D3DXMatrixInverse(&V, NULL, &V);

	float x = 0.0f;
	if (fixedY == false)
		x = asin(-V._32);

	float z = atan2(V._12, V._22);
	float y = atan2(V._31, V._33);

	//float x = asin(-V._32);
	//float z = atan2(V._12, V._22);

	//float y = 0.0f;
	//if (fixedY == false)
	//	y = atan2(V._31, V._33);

	quad->Rotation(x, y, z);
	quad->Update();
}

void BillBoard::Render()
{
	quad->Render();
}
