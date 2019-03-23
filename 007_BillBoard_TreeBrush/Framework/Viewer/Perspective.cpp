#include "Framework.h"
#include "Perspective.h"

Perspective::Perspective(float width, float height, float fov, float zn, float zf)
{
	Set(width, height, fov, zn, zf);
}

Perspective::~Perspective()
{
}

void Perspective::GetMatrix(D3DXMATRIX * mat)
{
	memcpy(mat, &projection, sizeof(D3DXMATRIX));
}

void Perspective::Set(float width, float height, float fov, float zn, float zf)
{
	this->width = width;
	this->height = height;
	this->fov = fov; // 수직시야각
	this->aspect = width / height; // 화면비율,,
	this->zn = zn;
	this->zf = zf;

	D3DXMatrixPerspectiveFovLH(&projection, fov, aspect, zn, zf);
}