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
	ID3DX11EffectVectorVariable* sColor; // ���� �̷���,, ���̴� �ѱ涧 �̸������س�������
};