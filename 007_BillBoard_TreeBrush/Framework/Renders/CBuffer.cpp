#include "Framework.h"
#include "CBuffer.h"

CBuffer::CBuffer(void * data, UINT dataSize)
	:data(data), dataSize(dataSize)
{
	D3D11_BUFFER_DESC desc = { 0 };
	desc.ByteWidth = dataSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // �������ִ°�
	desc.Usage = D3D11_USAGE_DYNAMIC; // DYNAMIC = cpu ����, gpu �б⸸ ����
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;// cpu ���� �÷���

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
	D3D::GetDC()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource); // D3D11_MAP_WRITE_DISCARD ������ ���ִ°�
	{
		memcpy(subResource.pData, data, dataSize);
	}
	D3D::GetDC()->Unmap(buffer, 0); // ���������� �ݵ�� ������� Ǯ����ߵ�
}