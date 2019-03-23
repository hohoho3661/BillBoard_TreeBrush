#include "stdafx.h"
#include "Test.h"
#include "Environment/Terrain.h"
#include "Environment/BillBoard.h"
#include "Environment/Sky.h"

void Test::Initialize()
{
	sky = new Sky;

	shader = new Shader(L"007_BillBoard_TreeBrush.fx");
	terrain = new Terrain(shader, L"HeightMaps/Gray256.png");
	terrain->BaseMap(L"Terrain/Dirt.png");
	terrain->LayerMap(L"Terrain/Grass (Lawn).jpg", L"Terrain/AlphaMap256.png");

	//
	type = 0;
	color = D3DXCOLOR(0, 1, 0, 1);
	range = 10;
	speed = 5.0f;
	wantHeight = 0.0f;
}

void Test::Ready()
{
}

void Test::Destroy()
{
}

void Test::Update()
{
	sky->Update();
	terrain->Update();


	//
	D3DXVECTOR3 picked = terrain->GetPickedHeight();

	ImGui::LabelText("Picking", "%.2f, %.2f, %.2f", picked.x, picked.y, picked.z);

	// TestBrush

	ImGui::SliderInt("Type", (int *)&type, 0, 3);
	ImGui::ColorEdit3("Color", (float *)&color);
	ImGui::SliderInt("Range", (int *)&range, 1, 50);
	ImGui::SliderFloat("Speed", (float *)&speed, 1.0f, 70.0f);
	ImGui::SliderInt("Height", (int *)&wantHeight, 0, 50);

	terrain->BrushLocation(picked);
	terrain->BrushColor(color);
	terrain->BrushRange(range);
	terrain->BrushType(type);


	if (Mouse::Get()->Down(0)&& type == 3) // ¸¶¿ì½º ¿ÞÂÊÅ¬¸¯
	{
		terrain->InsertBillBoard(picked, type, range);
	}
	if (Mouse::Get()->Press(0))
	{
		terrain->RaiseHeight(picked, type, range, speed, wantHeight);
	}
}


void Test::PreRender()
{
}

void Test::Render()
{
	sky->Render(); // sky ½¦ÀÌ´õ¿É¼Ç¿¡¼­ ±íÀÌ¿É¼ÇÀ» ²¨³ùÀ¸´Ï sky¸¦ Á© ¸ÕÀú ·£´õÇØ¾ßµÊ
	
	terrain->Render();
}

void Test::PostRender()
{
}

void Test::ResizeScreen()
{
}
