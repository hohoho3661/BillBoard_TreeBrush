#pragma once
class ModelRender
{
public:
	ModelRender(Shader* shader, Model* model);
	~ModelRender();

	void Update();
	void Render();

	void Position(float x, float y, float z);
	void Position(D3DXVECTOR3& vec);
	void Position(D3DXVECTOR3* vec);

	void Scale(float x, float y, float z);
	void Scale(D3DXVECTOR3& vec);
	void Scale(D3DXVECTOR3* vec);

	void Rotation(float x, float y, float z);
	void Rotation(D3DXVECTOR3& vec);
	void Rotation(D3DXVECTOR3* vec);

	void RotationDegree(float x, float y, float z);
	void RotationDegree(D3DXVECTOR3& vec);
	void RotationDegree(D3DXVECTOR3* vec);

	void Direction(D3DXVECTOR3* vec);
	void Up(D3DXVECTOR3* vec);
	void Right(D3DXVECTOR3* vec);

private:
	Shader* shader;
	Model* model;
	Renderer* renderer;

	D3DXMATRIX transforms[MODEL_MAX_TRANSFORMS];
};