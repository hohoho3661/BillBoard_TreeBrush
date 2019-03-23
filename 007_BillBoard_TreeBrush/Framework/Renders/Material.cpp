#include "Framework.h"
#include "Material.h"

Material::Material() // 생성자
{
	Initialize();
}

Material::Material(Shader * shader) // 생성자 오버로딩
{
	Initialize();

	SetShader(shader); // 쉐이더가 있으면 쉐이더를 세팅
}

void Material::Initialize() // 초기화
{
	name = L"";

	diffuseMap = NULL;
	specularMap = NULL;
	normalMap = NULL;

	buffer = new CBuffer(&colorDesc, sizeof(ColorDesc));
}

Material::~Material() // 소멸자에서 지워주고
{
	SAFE_DELETE(diffuseMap);
	SAFE_DELETE(specularMap);
	SAFE_DELETE(normalMap);

	SAFE_DELETE(buffer);
}

void Material::SetShader(Shader * shader) // 쉐이더 세팅
{
	//this->shader = shader;

	sBuffer = shader->AsConstantBuffer("CB_Material"); // 변수,, 버퍼

	sDiffuseMap = shader->AsSRV("DiffuseMap"); // 쉐이더리소스뷰
	sSpecularMap = shader->AsSRV("SpecularMap");
	sNormalMap = shader->AsSRV("NormalMap");
}

void Material::Ambient(D3DXCOLOR & color)
{
	colorDesc.Ambient = color;
}

void Material::Ambient(float r, float g, float b, float a)
{
	Ambient(D3DXCOLOR(r, g, b, a));
}

void Material::Diffuse(D3DXCOLOR & color)
{
	colorDesc.Diffuse = color;
}

void Material::Diffuse(float r, float g, float b, float a)
{
	Diffuse(D3DXCOLOR(r, g, b, a));
}

void Material::Specular(D3DXCOLOR & color)
{
	colorDesc.Specular = color;
}

void Material::Specular(float r, float g, float b, float a)
{
	Specular(D3DXCOLOR(r, g, b, a));
}

void Material::Shininess(float val)
{
	colorDesc.Shininess = val;
}

void Material::DiffuseMap(string file)
{
	DiffuseMap(String::ToWString(file)); // string이면 wstring으로 바꿔서 아래꺼 실행
}

void Material::DiffuseMap(wstring file)
{
	SAFE_DELETE(diffuseMap); // 기존꺼 있으면 지워주고

	diffuseMap = new Texture(file); // 세팅
}

void Material::SpecularMap(string file)
{
	SpecularMap(String::ToWString(file));
}

void Material::SpecularMap(wstring file)
{
	SAFE_DELETE(specularMap);

	specularMap = new Texture(file);
}

void Material::NormalMap(string file)
{
	NormalMap(String::ToWString(file));
}

void Material::NormalMap(wstring file)
{
	SAFE_DELETE(normalMap);

	normalMap = new Texture(file);
}

void Material::Render()
{
	sBuffer->SetConstantBuffer(buffer->Buffer()); // 버퍼 세팅

	if (diffuseMap != NULL) // 있는지 확인하고
		sDiffuseMap->SetResource(diffuseMap->SRV()); // 있으면 쉐이더리소스뷰에 텍스쳐 세팅

	if (specularMap != NULL)
		sSpecularMap->SetResource(specularMap->SRV());

	if (normalMap != NULL)
		sNormalMap->SetResource(normalMap->SRV());
}