#pragma once

class Mesh : public Renderer
{
public:
	typedef VertexTextureNormal MeshVertex;

public:
	Mesh(Shader* shader);
	virtual ~Mesh();

	void Render() override;
	
	void Color(D3DXCOLOR& val);
	void Color(float r, float g, float b);

protected:
	virtual void Create() = 0;
	
protected:
	MeshVertex* vertices;
	UINT* indices;

	UINT vertexCount, indexCount;

	D3DXCOLOR color;

private:
	ID3DX11EffectVectorVariable* sColor; // 원래 이렇게,, 쉐이더 넘길때 미리저장해놓으려고
};