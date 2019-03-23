#include "Framework.h"
#include "CBuffer.h"

CBuffer::CBuffer(void * data, UINT dataSize)
	:data(data), dataSize(dataSize)
{
	D3D11_BUFFER_DESC desc = { 0 };
	desc.ByteWidth = dataSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // 연결해주는거
	desc.Usage = D3D11_USAGE_DYNAMIC; // DYNAMIC = cpu 쓰기, gpu 읽기만 가능
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;// cpu 접근 플래그

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &buffer);
	assert(SUCCEEDED(hr));
}

CBuffer::~CBuffer()
{
	SAFE_RELEASE(buffer);
}

void CBuffer::Apply()
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource); // D3D11_MAP_WRITE_DISCARD 쓰고나서 없애는거
	{
		memcpy(subResource.pData, data, dataSize);
	}
	D3D::GetDC()->Unmap(buffer, 0); // 맵을했으면 반드시 언맵으로 풀어줘야됨
}