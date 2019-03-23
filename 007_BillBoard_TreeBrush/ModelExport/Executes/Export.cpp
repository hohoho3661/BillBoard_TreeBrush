#include "stdafx.h"
#include "Export.h"
#include "Assimp/Loader.h"

void Export::Initialize()
{
	Loader* loader = NULL; // 그냥 읽기만 할거니까 생성자에서 해버림

	////Tower
	//loader = new Loader();
	//loader->ReadFile(L"Tower/Tower.fbx"); // _Assets경로
	//loader->ExportMaterial(L"Tower/Tower"); // _Textures경로
	//loader->ExportMesh(L"Tower/Tower");

	tower = new Model();
	tower->ReadMaterial(L"Tower/Tower");
	tower->ReadMesh(L"Tower/Tower");

	towerShader = new Shader(L"020_Model.fx");
	towerRender = new ModelRender(towerShader, tower);
	towerRender->Scale(0.01f, 0.01f, 0.01f);
	//towerRender->Position(10, 10, 10);
	towerRender->Rotation(0, 0, 0);




	////Kachujin
	//loader = new Loader();
	//loader->ReadFile(L"Kachujin/Mesh.fbx");
	//loader->ExportMaterial(L"Kachujin/Mesh");



	////Tank
	//loader = new Loader();
	//loader->ReadFile(L"tank/tank.fbx");
	//loader->ExportMaterial(L"Tank/Tank", false);
	//loader->ExportMesh(L"Tank/Tank");

	tank = new Model();
	tank->ReadMaterial(L"Tank/Tank");
	tank->ReadMesh(L"Tank/Tank");

	tankShader = new Shader(L"020_Model.fx");
	for (UINT i = 0; i < 5; i++)
	{
		ModelRender* tankRender = new ModelRender(tankShader, tank);
		//tankRender->Position(Math::Random(-20.0f, 20.0f), 0, 0);
		tankRender->Position(i * 10, 0, 0);

		tankRenders.push_back(tankRender);
	}




	////car
	//loader = new Loader();
	//loader->ReadFile(L"Car/Car.obj");
	//loader->ExportMaterial(L"Car/Car", false);
	//loader->ExportMesh(L"Car/Car");

//	car = new Model();
//	car->ReadMaterial(L"Car/Car");
//	car->ReadMesh(L"Car/Car");

//	carShader = new Shader(L"020_Model.fx");
//	carRender = new ModelRender(carShader, car);
//	carRender->Position(7, 0, 0);
//	carRender->Scale(2, 2, 2);


}

void Export::Ready()
{
}

void Export::Destroy()
{
	SAFE_DELETE(tower);
	SAFE_DELETE(towerShader);
	SAFE_DELETE(towerRender);

	SAFE_DELETE(tank);
	SAFE_DELETE(tankShader);

	for (ModelRender* tankRender : tankRenders)
	{
		SAFE_DELETE(tankRender);
	}

//	SAFE_DELETE(car);
//	SAFE_DELETE(carShader);
//	SAFE_DELETE(carRender);
}

void Export::Update()
{
	towerRender->Update();

	static UINT count = 0;
	ImGui::SliderInt("Number", (int *)&count, 0, 5);

	float speed = 5.0f;
	D3DXVECTOR3 position;
	tankRenders[count]->Position(&position);

	if (Keyboard::Get()->Press(VK_UP))
		position.z += speed * Time::Delta();
	else if (Keyboard::Get()->Press(VK_DOWN))
		position.z -= speed * Time::Delta();

	if (Keyboard::Get()->Press(VK_RIGHT))
		position.x += speed * Time::Delta();
	else if (Keyboard::Get()->Press(VK_LEFT))
		position.x -= speed * Time::Delta();

	tankRenders[count]->Position(position);

	//D3DXMatrixReflect() 반사벡터

	for (ModelRender* tankRender : tankRenders)
	{
		tankRender->Update();
	}

//	carRender->Update();
}


void Export::PreRender()
{
}

void Export::Render()
{
	towerRender->Render();

	for (ModelRender* tankRender : tankRenders)
	{
		tankRender->Render();
	}
//	carRender->Render();
}

void Export::PostRender()
{
}

void Export::ResizeScreen()
{
}
