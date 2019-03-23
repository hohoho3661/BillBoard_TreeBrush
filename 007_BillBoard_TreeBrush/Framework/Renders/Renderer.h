#pragma once

class Renderer
{
public:
	Renderer();
	Renderer(Shader* shader);
	virtual ~Renderer();

	Shader* GetShader() { return shader; }
	virtual void SetShader(Shader* shader);

	UINT Pass() { return pass; }
	void Pass(UINT val) { pass = val; }

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

	virtual void Update(); // 자식클래스에서 고쳐쓰기 가능
	virtual void Render();

	void World(D3DXMATRIX& matrix) { frameDesc.World = matrix; } //
	D3DXMATRIX& World() { return frameDesc.World; }

private:
	void Initialize();

protected:
	virtual void UpdateWorld();

protected:
	void CreateVertexBuffer(void* data, UINT count, UINT stride, UINT offset);
	void CreateIndexBuffer(UINT* data, UINT count);

private:
	struct FrameDesc // shader변수 주고받음,, 반드시 16바이트로 맞춰줘야됨
	{
		D3DXMATRIX World;
		D3DXMATRIX View;
		D3DXMATRIX ViewInverse; // 카메라의 역행렬
		D3DXMATRIX Projection;
		D3DXMATRIX VP; // 뷰프로젝션 연산 미리해논거

		D3DXCOLOR LightColor;
		D3DXVECTOR3 LightDirection;
		D3DXVECTOR3 LightPosition;

		float Time;

		float Padding; // 16바이트로 맞춰주기위해
	} frameDesc;

protected:
	Shader* shader;
	UINT pass;

	UINT stride, offset;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

private:
	CBuffer* frameBuffer;
	ID3DX11EffectConstantBuffer* sFrameBuffer;

	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotation;

	D3DXVECTOR3 direction;
	D3DXVECTOR3 up;
	D3DXVECTOR3 right;
};