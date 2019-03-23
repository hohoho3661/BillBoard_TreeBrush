#pragma once

class Material
{
public:
	Material();
	Material(Shader* shader);
	~Material();

	void SetShader(Shader* shader);


	void Name(wstring val) { name = val; }
	wstring Name() { return name; }

	D3DXCOLOR Ambient() { return colorDesc.Ambient; } // 값 가져오기
	void Ambient(D3DXCOLOR& color); // 값 세팅
	void Ambient(float r, float g, float b, float a = 1.0f); // r,g,b,a세팅 (a는 없으면 1.0f)

	D3DXCOLOR Diffuse() { return colorDesc.Diffuse; }
	void Diffuse(D3DXCOLOR& color);
	void Diffuse(float r, float g, float b, float a = 1.0f);

	D3DXCOLOR Specular() { return colorDesc.Specular; }
	void Specular(D3DXCOLOR& color);
	void Specular(float r, float g, float b, float a = 1.0f);

	float Shininess() { return colorDesc.Shininess; }
	void Shininess(float val);


	Texture* DiffuseMap() { return diffuseMap; } // 값 가져오기
	void DiffuseMap(string file);
	void DiffuseMap(wstring file);

	Texture* SpecularMap() { return specularMap; }
	void SpecularMap(string file);
	void SpecularMap(wstring file);

	Texture* NormalMap() { return normalMap; }
	void NormalMap(string file);
	void NormalMap(wstring file);

	void Render();

private:
	void Initialize();

private:
	struct ColorDesc
	{ //16배수로 맞춰야됨
		D3DXCOLOR Ambient = D3DXCOLOR(0, 0, 0, 1); // 16
		D3DXCOLOR Diffuse = D3DXCOLOR(1, 1, 1, 1); // 16
		D3DXCOLOR Specular = D3DXCOLOR(0, 0, 0, 1); // 16

		float Shininess = 0; // 4
		float Padding[3]; // 12
	} colorDesc;

private:
	Shader* shader;

	wstring name;

	Texture* diffuseMap;
	Texture* specularMap;
	Texture* normalMap;

	CBuffer* buffer;

	ID3DX11EffectConstantBuffer* sBuffer;
	ID3DX11EffectShaderResourceVariable* sDiffuseMap;
	ID3DX11EffectShaderResourceVariable* sSpecularMap;
	ID3DX11EffectShaderResourceVariable* sNormalMap;

	//Shader* shader;
};