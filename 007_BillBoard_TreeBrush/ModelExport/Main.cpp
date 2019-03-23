#include "stdafx.h"
#include "Main.h"
#include "Systems/Window.h"
#include "Viewer/Freedom.h"
#include "Executes/Export.h"

void Main::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(0,0);
	Context::Get()->GetCamera()->Position(0,5,-20);
	((Freedom*)Context::Get()->GetCamera())->Speed(20, 0.5f);
	
	Push(new Export());
}

void Main::Ready()
{
}

void Main::Destroy()
{
	for (IExecute* exe : executes)
	{
		exe->Destroy();
		SAFE_DELETE(exe);
	}
}

void Main::Update()
{
	Context::Get()->Update();

	for (IExecute* exe : executes)
		exe->Update();
}

void Main::PreRender()
{
	for (IExecute* exe : executes)
		exe->PreRender();
}

void Main::Render()
{
	string str = string("Frame Rate : ") + to_string(ImGui::GetIO().Framerate);
	Gui::Get()->RenderText(5, 5, 1, 1, 1, str);

	D3DXVECTOR3 camPos;
	Context::Get()->GetCamera()->Position(&camPos);

	D3DXVECTOR2 camDir;
	Context::Get()->GetCamera()->RotationDegree(&camDir);

	str = "Cam Position : ";
	str += to_string((int)camPos.x) + ", " + to_string((int)camPos.y) + ", " + to_string((int)camPos.z);
	Gui::Get()->RenderText(5, 20, 1, 1, 1, str);

	str = "Cam Rotation : ";
	str += to_string((int)camDir.x) + ", " + to_string((int)camDir.y);
	Gui::Get()->RenderText(5, 35, 1, 1, 1, str);

	for (IExecute* exe : executes)
		exe->Render();
}

void Main::PostRender()
{
	for (IExecute* exe : executes)
		exe->PostRender();
}

void Main::ResizeScreen()
{
	for (IExecute* exe : executes)
		exe->ResizeScreen();
}

void Main::Push(IExecute * execute)
{
	executes.push_back(execute);

	execute->Initialize();
}


int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR param, int command)
{
	D3DDesc desc;
	desc.AppName = L"D3D Game"; // ����ǥ���ٿ� ��Ÿ�� �̸�
	desc.Instance = instance; // ���α׷� �����ּ�, �ĺ���
	desc.bFullScreen = false; // ��üȭ��
	desc.bVsync = false; // ��������ȭ (60����)
	desc.Handle = NULL; // ���������
	desc.Width = 1280;
	desc.Height = 720;
	desc.Background = D3DXCOLOR(0xFF6B6562);
	D3D::SetDesc(desc);


	Main* main = new Main();
	WPARAM wParam = Window::Run(main);

	SAFE_DELETE(main);

	return wParam;
}