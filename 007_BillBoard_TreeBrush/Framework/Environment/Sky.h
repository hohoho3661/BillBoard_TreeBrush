#pragma once
// 깊이 판단이 없으면 그리는 순서대로 보여줌
// Sky는 깊이를 없앨거임
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