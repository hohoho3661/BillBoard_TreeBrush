#pragma once

class CBuffer
{
public:
	CBuffer(void* data, UINT dataSize);
	~CBuffer();

	void Apply();

	ID3D11Buffer* Buffer() { return buffer; }

private:
	ID3D11Buffer* buffer;

	void* data;
	UINT dataSize;
};