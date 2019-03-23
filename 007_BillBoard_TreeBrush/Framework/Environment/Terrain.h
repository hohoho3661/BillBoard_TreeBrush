#pragma once

class Terrain : public Renderer
{
public:
	typedef VertexTextureNormal TerrainVertex;

public:
	Terrain(Shader* shader, wstring heightMap);
	~Terrain();

	void Update() override;
	void Render() override;

	void BrushColor(D3DXCOLOR& color) { brushDesc.Color = color; } // 외부에서 값세팅
	void BrushLocation(D3DXVECTOR3& vec) { brushDesc.Location = vec; }
	void BrushType(UINT val) { brushDesc.Type = val; }
	void BrushRange(UINT val) { brushDesc.Range = val; }

public:
	float GetHeight(D3DXVECTOR3& position); // 모든맵크기가 같을때
	float GetPickedHeight(D3DXVECTOR3& position); // 다를때 (실무에서 쓰는방식)
	D3DXVECTOR3 GetPickedHeight();

	void RaiseHeight(D3DXVECTOR3& position, UINT type, UINT range
		, float speed, float wantHeight); // 높이조정
	
	void InsertBillBoard(D3DXVECTOR3& position, UINT type, UINT range);

	void BaseMap(wstring file);
	void LayerMap(wstring file, wstring alpha);

public:
	void Save(wstring savePath);
	void Load(wstring savePath);

private:
	void CreateVertexDate();
	void CreateIndexDate();
	void CreateNormalDate();

private:
	struct BrushDesc
	{
		D3DXCOLOR Color = D3DXCOLOR(0, 1, 0, 1);
		D3DXVECTOR3 Location = D3DXVECTOR3(-1, -1, -1);
		int Type = 0;
		int Range = 1;

		float Padding[3];
	} brushDesc;

	struct GridLineDese
	{
		D3DXCOLOR Color = D3DXCOLOR(1, 1, 1, 1);

		int Visible = 1;
		float Thickness = 0.1f;
		float Size = 25.5;

		float Padding;
	} gridLineDesc;
private:
	
	D3DXMATRIX world;		
	Texture* heightMap;

	UINT width, height;

	UINT vertexCount;
	TerrainVertex* vertices;

	UINT indexCount;
	UINT* indices;


	//
	CBuffer* brushBuffer;
	ID3DX11EffectConstantBuffer* sBrushBuffer;

	//
	CBuffer* gridLineBuffer;
	ID3DX11EffectConstantBuffer* sGridLineBuffer;

	//
	Texture* baseMap;
	ID3DX11EffectShaderResourceVariable* sBaseMap;

	Texture* layerMap;
	ID3DX11EffectShaderResourceVariable* sLayerMap;

	Texture* alphaMap;
	ID3DX11EffectShaderResourceVariable* sAlphaMap;

private:
	//빌보드
	Shader* billShader;
	vector< class BillBoard*> billBoards;
};