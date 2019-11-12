#include "Dx11_Ocean.h"


Dx11_Ocean::Dx11_Ocean()
{
	m_fTime = 0.0f;
	m_pTexture = new Dx11_Texture();
}


Dx11_Ocean::~Dx11_Ocean()
{
}


void Dx11_Ocean::Shutdown()
{

}

bool Dx11_Ocean::InitializeOcean(ID3D11Device *pDevice, WCHAR *pTextureFile)
{
	if (m_pTexture->Initiazlize(pDevice, pTextureFile) == false)
	{
		MessageBox(NULL, L"Ocean Texture Fail to Load", L"Error", MB_ICONERROR | MB_OK);
	}

	stOceanVertex *pVertices;
	unsigned int  *pIndices;

	int nPoints = 100;
	int nVertexCount =  nPoints * nPoints;
	m_nIndexCount = (nPoints - 1)* (nPoints - 1) * 6;
	pVertices = new stOceanVertex[nVertexCount];
	pIndices = new unsigned int[m_nIndexCount];

	int r, c, index = 0;
	for (c = 0; c < nPoints; c++)
	{
		for (r = 0; r < nPoints; r++)
		{
			pVertices[index].pos = D3DXVECTOR3((float)c, 0, (float)(r));
			pVertices[index].col = D3DXVECTOR4(0.6f, 0.6f, 1.0f, 1.0f);
			pVertices[index].normal = D3DXVECTOR3(0, 1, 0);
			index++;
		}
	}

	index = 0;
	for (r = 0; r < nPoints - 1; r++)
	{
		for (c = 0; c < nPoints - 1; c++)
		{
			pIndices[index++] = r*nPoints + c;
			pIndices[index++] = r*nPoints + (c + 1);
			pIndices[index++] = (r + 1)*nPoints + c;

			pIndices[index++] = (r + 1)*nPoints + c;
			pIndices[index++] = r*nPoints + (c + 1);
			pIndices[index++] = (r + 1)*nPoints + (c + 1);
		}
	}
	D3D11_BUFFER_DESC desc, indxDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(stOceanVertex)* nVertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	HRESULT hr = pDevice->CreateBuffer(&desc, &vertexData, &m_pVB);
	if (hr != S_OK)
		return false;

	ZeroMemory(&indxDesc, sizeof(indxDesc));
	indxDesc.Usage = D3D11_USAGE_DEFAULT;
	indxDesc.ByteWidth = sizeof(unsigned int)* m_nIndexCount;
	indxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indxDesc.CPUAccessFlags = 0;
	indxDesc.MiscFlags = 0;
	indxDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	hr = pDevice->CreateBuffer(&indxDesc, &indexData, &m_pIB);
	if (hr != S_OK)
		return false;


	delete[] pVertices;
	delete[] pIndices;
	pVertices = 0;
	pIndices = 0;


	return true;
}


void Dx11_Ocean::Render(ID3D11DeviceContext *pDeviceContext)
{
	if (pDeviceContext)
	{
		UINT stride = sizeof(stOceanVertex);
		UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}


void Dx11_Ocean::Process(float _fTick)
{
	m_fTime += _fTick;
}