#pragma once
// ���� �Ǵ��� ������ �׸��� ������� ������
// Sky�� ���̸� ���ٰ���
class Sky
{
public:
	Sky();
	~Sky();

	void Update();
	void Render();

private:
	struct DataDesc
	{
		//D3DXCOLOR Center = D3DXCOLOR(0xFF0080FF);
		//D3DXCOLOR Apex = D3DXCOLOR(0xFF9BCDFF);
		D3DXCOLOR Center = D3DXCOLOR(0xFF9BCDFF);
		D3DXCOLOR Apex = D3DXCOLOR(0xFF0080FF);
		
		float Height = 2.0f;
		//float Padding[3];
	} dataDesc;

private:
	Shader* shader;
	MeshSphere* sphere;

	ID3DX11EffectVariable* sData;
	
};