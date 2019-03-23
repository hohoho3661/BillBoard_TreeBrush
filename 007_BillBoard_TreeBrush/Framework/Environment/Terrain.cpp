#include "Framework.h"
#include "terrain.h"
#include "Viewer/Viewport.h"
#include "BillBoard.h"
#include "Utilities/Xml.h"

Terrain::Terrain(Shader * shader, wstring heightMap)
	: Renderer(shader)
	, baseMap(NULL), layerMap(NULL), alphaMap(NULL)
{
	pass = 0;

	this->heightMap = new Texture(heightMap);

	D3DXMatrixIdentity(&world);

	CreateVertexDate();
	CreateIndexDate();
	CreateNormalDate();

	CreateVertexBuffer(vertices, vertexCount, sizeof(TerrainVertex), 0);
	CreateIndexBuffer(indices, indexCount);


	brushBuffer = new CBuffer(&brushDesc, sizeof(brushDesc));
	sBrushBuffer = shader->AsConstantBuffer("CB_Brush");

	gridLineBuffer = new CBuffer(&gridLineDesc, sizeof(gridLineDesc));
	sGridLineBuffer = shader->AsConstantBuffer("CB_GridLine");

	sBaseMap = shader->AsSRV("BaseMap");
	sLayerMap = shader->AsSRV("LayerMap");
	sAlphaMap = shader->AsSRV("AlphaMap");


	//빌보드
	billShader = new Shader(L"021_BillBoard.fx");
	
}

Terrain::~Terrain()
{
	SAFE_DELETE(baseMap);
	SAFE_DELETE(layerMap);
	SAFE_DELETE(alphaMap);

	SAFE_DELETE(brushBuffer);
	SAFE_DELETE(gridLineBuffer);

	SAFE_DELETE(heightMap);

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);
}

void Terrain::Update()
{
	//__super::Update(); // 바로위에 부모의 함수를 콜해줌
	Renderer::Update();

	for (BillBoard* b : billBoards)
		b->Update();


	// Save, Load 버튼만들기

	if (ImGui::Button("Save") == true)
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		function<void(wstring)> func = bind(&Terrain::Save, this, placeholders::_1);
		Path::SaveFileDialog(L"", NULL, L"../../_SaveXml/", func, desc.Handle);
	}

	if (ImGui::Button("Load") == true)
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		function<void(wstring)> func = bind(&Terrain::Load, this, placeholders::_1);
		Path::OpenFileDialog(L"", NULL, L"../../_SaveXml/", func, desc.Handle);
	}
	if (ImGui::Button("Clear") == true)
	{
		for (BillBoard* b : billBoards)
		{
			SAFE_DELETE(b);
		}
		billBoards.clear();
	}
}

void Terrain::Render()
{
	Renderer::Render();

	if (sBrushBuffer != NULL)
	{
		brushBuffer->Apply();
		sBrushBuffer->SetConstantBuffer(brushBuffer->Buffer());
	}

	if (sGridLineBuffer != NULL)
	{
		gridLineBuffer->Apply();
		sGridLineBuffer->SetConstantBuffer(gridLineBuffer->Buffer());
	}

	if (baseMap != NULL)
		sBaseMap->SetResource(baseMap->SRV());

	if (layerMap != NULL && alphaMap != NULL)
	{
		sLayerMap->SetResource(layerMap->SRV());
		sAlphaMap->SetResource(alphaMap->SRV());
	}

	
	ImGui::SliderInt("Pass", (int*)&pass, 0, 1);
	shader->DrawIndexed(0, pass, indexCount);


	for (BillBoard* b : billBoards)
		b->Render();
}

float Terrain::GetHeight(D3DXVECTOR3 & position) // 외부에서 플레이어가 있는포지션
{
	UINT x = (UINT)position.x; // 정점번호에 소수점을 없애려고 UINT로
	UINT z = (UINT)position.z;

	if (x < 0 || x > width) return -1.0f;
	if (z < 0 || z > height) return -1.0f;

	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + (x + 1);
	index[3] = width * (z + 1) + (x + 1);

	D3DXVECTOR3 v[4];
	for (int i = 0; i < 4; i++)
	{
		v[i] = vertices[index[i]].Position;
	}

	// 기울기 (비율) 구해보자
	float ddx = (position.x - v[0].x) / 1.0f;
	float ddz = (position.z - v[0].z) / 1.0f;

	D3DXVECTOR3 temp;
	if (ddx + ddz <= 1)
	{
		temp = v[0] + (v[2] - v[0]) * ddx
			+ (v[1] - v[0]) * ddz;
	}
	else
	{
		ddx = 1 - ddx;
		ddz = 1 - ddz;

		temp = v[3] + (v[1] - v[3]) * ddx
			+ (v[2] - v[3]) * ddz;
	}


	return temp.y;
}

float Terrain::GetPickedHeight(D3DXVECTOR3 & position)
{
	UINT x = (UINT)position.x; // 정점번호에 소수점을 없애려고 UINT로
	UINT z = (UINT)position.z;

	if (x < 0 || x > width) return -1.0f;
	if (z < 0 || z > height) return -1.0f;

	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + (x + 1);
	index[3] = width * (z + 1) + (x + 1);

	D3DXVECTOR3 p[4];
	for (int i = 0; i < 4; i++)
	{
		p[i] = vertices[index[i]].Position;
	}


	//
	float u, v, distance;

	D3DXVECTOR3 start(position.x, 1000, position.z); // 1000만큼 충분히 높은곳에서 내려오게
	D3DXVECTOR3 direction(0, -1, 0); // 내려오는방향

	D3DXVECTOR3 result(-1, -1, -1);
	if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
	{
		result = p[0] + (p[1] - p[0]) * u
			+ (p[2] - p[0]) * v;
	}
	if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
	{
		result = p[3] + (p[1] - p[3]) * u
			+ (p[2] - p[3]) * v;
	}

	return result.y;
}

D3DXVECTOR3 Terrain::GetPickedHeight()
{
	D3DXVECTOR3 start, direction;
	D3DXMATRIX V = Context::Get()->View();
	D3DXMATRIX P = Context::Get()->Projection();

		
	D3DXVECTOR3 mouse = Mouse::Get()->GetPosition();

	D3DXVECTOR3 n, f;

	mouse.z = 0.0f;
	Context::Get()->GetViewport()->Unproject(&n, mouse, world, V, P);

	mouse.z = 1.0f;
	Context::Get()->GetViewport()->Unproject(&f, mouse, world, V, P);

	direction = f - n;
	start = n;

	//Context::Get()->GetViewport()->GetRay(&start, &direction, world, V, P);

	for (UINT z = 0; z < height - 1; z++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			UINT index[4];
			index[0] = width * z + x;
			index[1] = width * (z + 1) + x;
			index[2] = width * z + (x + 1);
			index[3] = width * (z + 1) + (x + 1);

			D3DXVECTOR3 p[4];
			for (int i = 0; i < 4; i++)
			{
				p[i] = vertices[index[i]].Position;
			}
			
			//
			float u, v, distance;

			D3DXVECTOR3 result(-1, -1, -1);
			if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
			{
				return p[0] + (p[1] - p[0]) * u
					+ (p[2] - p[0]) * v;
			}
			if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
			{
				return p[3] + (p[1] - p[3]) * u
					+ (p[2] - p[3]) * v;
			}
		}
	}


	return D3DXVECTOR3(-1, -1, -1); // 아무것도 피킹이 안됬을때
}

void Terrain::RaiseHeight(D3DXVECTOR3 & position, UINT type, UINT range, float speed, float wantHeight)
{
	D3D11_BOX box; // 범위
	box.left = (UINT)position.x - range;
	box.top = (UINT)position.z + range;
	box.right = (UINT)position.x + range;
	box.bottom = (UINT)position.z - range;

	if (box.left < 0)
		box.left = 0;
	if (box.top >= height)
		box.left = height;
	if (box.right >= width)
		box.right = width;
	if (box.bottom < 0)
		box.bottom = 0;

	// 정점 검사
	for (UINT z = box.bottom; z <= box.top; z++)
	{
		for (UINT x = box.left; x <= box.right; x++)
		{
			UINT index = width*z + x;

			// 올라갈지 내려갈지 방향
			int dir = 1;
			if (Keyboard::Get()->Press(VK_SHIFT))
				dir = -1;
			if (!(Keyboard::Get()->Press(VK_SHIFT)))
				dir = 1;

			if (type == 1) // 사각형모양으로 검사
			{
				if (!(Keyboard::Get()->Press(VK_CONTROL)))
				{
					vertices[index].Position.y += speed * dir * Time::Delta();
				}
				if (Keyboard::Get()->Press(VK_CONTROL))
				{
					vertices[index].Position.y = wantHeight;
				}
			}
			else if (type == 2) // 원모양으로 검사
			{
				float dx = position.x - x;
				float dz = position.z - z;
				float dist = sqrtf(dx * dx + dz * dz);

				if (dist <= range)
				{
					if (!(Keyboard::Get()->Press(VK_CONTROL)))
					{
						vertices[index].Position.y += speed * dir * Time::Delta();
					}
					if (Keyboard::Get()->Press(VK_CONTROL))
					{
						vertices[index].Position.y = wantHeight;
					}
				}
			}

			// 바닥이 0보다 낮게되는걸 방지
			if (vertices[index].Position.y <= 0)
				vertices[index].Position.y = 0;
		}
	}

	CreateNormalDate();

	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, vertices, sizeof(TerrainVertex) * vertexCount, 0);
	
}

void Terrain::InsertBillBoard(D3DXVECTOR3 & position, UINT type, UINT range)
{
	D3D11_BOX box; // 범위
	box.left = (UINT)position.x - range;
	box.top = (UINT)position.z + range;
	box.right = (UINT)position.x + range;
	box.bottom = (UINT)position.z - range;

	if (box.left < 0)
		box.left = 0;
	if (box.top >= height)
		box.left = height;
	if (box.right >= width)
		box.right = width;
	if (box.bottom < 0)
		box.bottom = 0;
	
	if (type == 3) // 나무심기
	{
		for (UINT i = 0; i < 5; i++)
		{
			D3DXVECTOR3 scale;
			scale.x = Math::Random(5.0f, 15.0f);
			scale.y = Math::Random(5.0f, 15.0f);
			scale.z = 1.0f;

			D3DXVECTOR3 billPosition;
			billPosition.x = Math::Random(position.x - range, position.x + range);
			billPosition.z = Math::Random(position.z - range, position.z + range);
			billPosition.y = GetHeight(billPosition) + (scale.y * 0.5f);

			BillBoard* billboard = new BillBoard(billShader, L"Tree.png");
			billboard->Position(billPosition);
			billboard->Scale(scale);

			billBoards.push_back(billboard);
		}
	}
}

void Terrain::BaseMap(wstring file)
{
	SAFE_DELETE(baseMap);

	baseMap = new Texture(file);
}

void Terrain::LayerMap(wstring file, wstring alpha)
{
	SAFE_DELETE(layerMap);
	SAFE_DELETE(alphaMap);

	layerMap = new Texture(file);
	alphaMap = new Texture(alpha);
}

void Terrain::Save(wstring savePath)
{
	//if (bOverwrite == false) // 덮어쓰지 않겟다고하면
	//{
	//	if (Path::ExistFile(savePath) == true) // 파일이 존재하는지 확인하고
	//		return;							   // 존재하면 그냥 return으로 넘김
	//}

	string folder = String::ToString(Path::GetDirectoryName(savePath));
	string file = String::ToString(Path::GetFileName(savePath + L".xml"));

	Path::CreateFolders(folder);

	// Xml,,   Element는 밑으로, Attribute는 옆으로
	Xml::XMLDocument* document = new Xml::XMLDocument(); // xml은 document(문서)로 부터 시작됨,, 항상 이게있어야됨

	Xml::XMLDeclaration* decl = document->NewDeclaration(); // xml파일을 사용하겟다. 선언해줌
	document->LinkEndChild(decl); // LinkEndChild = 밑에 붙이라는 말임,,  cf)최상단은 document

	Xml::XMLElement* root = document->NewElement("Tree"); //NewElement(이름) = 만들어줌,, root부터 시작하자
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance"); // xml형식을 공식적으로 정의해놓은 사이트
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");		  // 이걸 반드시 해줘야 xml형식으로 읽어짐
	document->LinkEndChild(root);
	//---------- 여까지는 공식이라고 생각하자

	for (BillBoard* b : billBoards)
	{
		Xml::XMLElement* node = document->NewElement("BillBoard");
		root->LinkEndChild(node);

		Xml:XmlElement* element = NULL;

		D3DXVECTOR3 p;
		D3DXVECTOR3 s;

		b->Position(&p);
		b->Scale(&s);

		element = document->NewElement("Position");
		element->SetAttribute("position.x", p.x);
		element->SetAttribute("position.y", p.y);
		element->SetAttribute("position.z", p.z);
		node->LinkEndChild(element);

		element = document->NewElement("Scale");
		element->SetAttribute("scale.x", s.x);
		element->SetAttribute("scale.y", s.y);
		element->SetAttribute("scale.z", s.z);
		node->LinkEndChild(element);
	}

	document->SaveFile((folder + file).c_str());
}

void Terrain::Load(wstring savePath)
{
	//file = L"../../_Textures/" + file + L".material"; // 파일이름에 경로랑 .material붙게

	Xml::XMLDocument* document = new Xml::XMLDocument(); // document만들고
	Xml::XMLError error = document->LoadFile(String::ToString(savePath).c_str()); // 파일불러서 에러체크,, 정상적으로 불러졌는지
	assert(error == Xml::XML_SUCCESS);	// XML_SUCCESS = 0

	//xml에 있는 element불러오기
	Xml::XMLElement* root = document->FirstChildElement();	   // root는 document의 첫번째꺼,, (Materials)
	Xml::XMLElement* Info = root->FirstChildElement(); // materialNode는 root의 첫번째꺼 (Material)

	do
	{
		//Material* material = new Material(); // 우리꺼 저장할 material를 만들어 놓고

		Xml::XMLElement* node = NULL; // 값을 받아올 node를 만들어 놓고
		node = Info->FirstChildElement();
		D3DXVECTOR3 p;
		D3DXVECTOR3 s;

		p.x = node->FloatAttribute("position.x");
		p.y = node->FloatAttribute("position.y");
		p.z = node->FloatAttribute("position.z");
		node = node->NextSiblingElement();

		s.x = node->FloatAttribute("scale.x");
		s.y = node->FloatAttribute("scale.y");
		s.z = node->FloatAttribute("scale.z");
		node = node->NextSiblingElement();


		BillBoard* b = new BillBoard(billShader, L"Tree.png");
		b->Position(p);
		b->Scale(s);
		billBoards.push_back(b);

		Info = Info->NextSiblingElement();
	} while (Info != NULL); // materialNode가 NULL이 아닐때까지 위에 반복함
}

void Terrain::CreateVertexDate()
{
	//
	vector<D3DXCOLOR> heights; // 색갈로 높이를 꺼내오자
	heightMap->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);

	width = heightMap->GetWidth();
	height = heightMap->GetHeight();
	//


	vertexCount = width * height;
	vertices = new TerrainVertex[vertexCount];

	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index = width * z + x;
			UINT pixel = width *(height - 1 - z) + x;

			vertices[index].Position.x = (float)x;
			vertices[index].Position.y = (heights[pixel].r * 255.0f) / 10.0f; // height로 리턴되는값은 0~1
			vertices[index].Position.z = (float)z;

			vertices[index].Uv.x = (float)x / (float)width;
			vertices[index].Uv.y = (float)(height - 1 - z) / (float)height;
		}
	}
}

void Terrain::CreateIndexDate()
{	
	// 인덱스
	indexCount = (width - 1) * (height - 1) * 6;
	indices = new UINT[indexCount];

	UINT index = 0;
	for (UINT z = 0; z < height - 1; z++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			indices[index + 0] = width * z + x;
			indices[index + 1] = width * (z + 1) + x;
			indices[index + 2] = width * z + x + 1;
			indices[index + 3] = width * z + x + 1;
			indices[index + 4] = width * (z + 1) + x;
			indices[index + 5] = width * (z + 1) + x + 1;

			index += 6;
		}
	}
}

void Terrain::CreateNormalDate()
{
	for (UINT i = 0; i < indexCount / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0]; // 각 삼각형의 0번째 정점의 번호
		UINT index1 = indices[i * 3 + 1]; 
		UINT index2 = indices[i * 3 + 2]; 

		TerrainVertex v0 = vertices[index0]; // 해당 정점을 가져올수있음
		TerrainVertex v1 = vertices[index1];
		TerrainVertex v2 = vertices[index2];

		D3DXVECTOR3 d1 = v1.Position - v0.Position;
		D3DXVECTOR3 d2 = v2.Position - v0.Position;

		D3DXVECTOR3 normal; //수직벡터 저장할 변수
		D3DXVec3Cross(&normal, &d1, &d2); // 순서중요

		vertices[index0].Normal += normal; // 누적
		vertices[index1].Normal += normal;
		vertices[index2].Normal += normal;
	}

	for (UINT i = 0; i < vertexCount; i++)
		D3DXVec3Normalize(&vertices[i].Normal, &vertices[i].Normal);
}