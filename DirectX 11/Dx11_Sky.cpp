#include "Dx11_Sky.h"


Dx11_Sky::Dx11_Sky()
{
	
	m_nResTheta = 32;
	m_nResPhi = 16;

	m_pTexture = new Dx11_Texture;

	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;

	m_nRadius = 300;
}


Dx11_Sky::~Dx11_Sky()
{
}


bool Dx11_Sky::InitSky(ID3D11Device *pDevice, WCHAR *pCloudTexture)
{

	m_nVertexCount = m_nResTheta * m_nResPhi;	
	st_SKY_VERTEX *pSkyVertex = new st_SKY_VERTEX[m_nVertexCount];


	//SPHERICAL COORDINATE SYSTEM
	int index = 0, r, c;
	float fTheta, fPhi, fDeltaTheta, fDeltaPhi;
	

	fDeltaPhi =  (D3DX_PI) / (float)(m_nResPhi - 1);
	fDeltaTheta = (2 * D3DX_PI ) / (float)(m_nResTheta - 1);
	
	fTheta = 0;
	for (r = 0; r < m_nResTheta /*No of Points On Longitude*/; r++)
	{
		fPhi = 0;
		for (c = 0; c <m_nResPhi /*No of Points On Latitude */; c++)
		{			
			pSkyVertex[index].pos = m_nRadius * D3DXVECTOR3(sinf(fPhi)*cosf(fTheta), cosf(fPhi), sinf(fPhi)*sinf(fTheta));
			//pSkyVertex[index].pos = D3DXVECTOR3((float)c, 0, (float)(r));
			pSkyVertex[index].diffuse = D3DXCOLOR(153/255.0f, 189/255.0f, 230/255.0f, 1.0f);
			pSkyVertex[index].tex = D3DXVECTOR2((float)(c + 1) / (float)m_nResPhi, (float)(r + 1) / (float)m_nResTheta);
			//pSkyVertex[index].tex = D3DXVECTOR2((float)(c + 1) / (float)m_nResPhi, (float)(r + 1) / (float)m_nResTheta);

			/*
			D3DXVECTOR3 n;
			D3DXVec3Normalize(&n, &pSkyVertex[index].pos);
			float u = atan2(n.x, n.z) / (2 * D3DX_PI) + 0.5;
			float v = n.y*0.5 + 0.5;
			pSkyVertex[index].tex = D3DXVECTOR2(u, v);			
			*/
			index++;
			fPhi += fDeltaPhi;
		}	
		fTheta += fDeltaTheta;		
	}

	
	D3D11_BUFFER_DESC vertexBuffDesc, indexBuffDesc;
	D3D11_SUBRESOURCE_DATA vertexSubData, indexSubData;
	ZeroMemory(&vertexBuffDesc, sizeof(vertexBuffDesc));
	vertexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBuffDesc.ByteWidth = sizeof(st_SKY_VERTEX)* m_nVertexCount;
	vertexBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBuffDesc.CPUAccessFlags = 0;
	vertexBuffDesc.MiscFlags = 0;
	vertexBuffDesc.StructureByteStride = 0;

	ZeroMemory(&vertexSubData, sizeof(vertexSubData));
	vertexSubData.pSysMem = pSkyVertex;	
	vertexSubData.SysMemPitch = 0;
	vertexSubData.SysMemSlicePitch = 0;
	HRESULT hr = pDevice->CreateBuffer(&vertexBuffDesc, &vertexSubData, &m_pVertexBuffer);
	if (hr != S_OK)
		return false;


	m_nIndexCount = (m_nResTheta - 1) * (m_nResPhi - 1) *(6);
	//m_nIndexCount = (m_nResTheta ) * (m_nResPhi) *(6);
	unsigned long *pIndices = new unsigned long[m_nIndexCount];
	index = 0;
	for (r = 0; r<m_nResTheta-1; r++)
	{
		for (c = 0; c<m_nResPhi -1; c++)
		{
			//FIRST 
			pIndices[index++] = r*m_nResPhi + c;
			pIndices[index++] = r*m_nResPhi + (c + 1);
			pIndices[index++] = (r + 1)*m_nResPhi + c;

			//SECOND 
			//if (index + 1 % m_nResPhi != 0)
			{
				pIndices[index++] = (r + 1)*m_nResPhi + c;
				pIndices[index++] = r*m_nResPhi + (c + 1);
				pIndices[index++] = (r + 1)*m_nResPhi + (c + 1);
			}
			//else
				//index = index + 3;		

		}
	}
	
	ZeroMemory(&indexBuffDesc, sizeof(indexBuffDesc));
	indexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBuffDesc.ByteWidth = sizeof(unsigned long)* m_nIndexCount;
	indexBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBuffDesc.CPUAccessFlags = 0;
	indexBuffDesc.MiscFlags = 0;
	indexBuffDesc.StructureByteStride = 0;


	ZeroMemory(&indexSubData, sizeof(indexSubData));
	indexSubData.pSysMem = pIndices;
	indexSubData.SysMemPitch = 0;
	indexSubData.SysMemSlicePitch = 0;
	hr = pDevice->CreateBuffer(&indexBuffDesc, &indexSubData, &m_pIndexBuffer);
	if (hr != S_OK)
		return false;

	//delete []pIndices;
	//delete []pSkyVertex;

	if (!m_pTexture->Initiazlize(pDevice, pCloudTexture))
	{
		MessageBox(NULL, L"FAIL : Cloud Texture", L"Warring", MB_ICONEXCLAMATION);
		return false;
	}

	return true;

}


void Dx11_Sky::Shutdown()
{
	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = 0;
	}

	if (m_pVertexBuffer)
	{		
		m_pVertexBuffer->Release();
		m_pVertexBuffer = 0;
	}

	if (m_pTexture)
		m_pTexture->Shutdown();
}


void Dx11_Sky::Render(ID3D11DeviceContext *pDeviceContext)
{

	if (pDeviceContext)
	{
		UINT stride = sizeof(st_SKY_VERTEX);
		UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}


void Dx11_Sky::Process(D3DXVECTOR3 vCameraPos, D3DXVECTOR3 vCameraFrwdDir)
{

}