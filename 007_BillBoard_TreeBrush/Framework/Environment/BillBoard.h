#pragma once

class BillBoard : public Renderer
{
public:
	BillBoard(Shader* shader, wstring file);
	~BillBoard();

	void FixedY(bool val) { fixedY = val; }

	void Position(float x, float y, float z);
	void Position(D3DXVECTOR3 vec);
	void Position(D3DXVECTOR3* vec);

	void Scale(float x, float y, float z);
	void Scale(D3DXVECTOR3 vec);
	void Scale(D3DXVECTOR3* vec);

	void Update();
	void Render();


private:
	bool fixedY;

	Shader* shader;
	MeshQuad* quad;
	//vector<MeshQuad*> quads;

	Texture* texture;
};