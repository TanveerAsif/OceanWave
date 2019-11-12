#include "Dx11_ParticleSystem.h"


Dx11_ParticleSystem::Dx11_ParticleSystem()
{
	
	m_pTexture = new Dx11_Texture();
	m_pParticleList = NULL;
	m_pVB = NULL;
	m_pIB = NULL;

	m_pVertices = NULL;
}


Dx11_ParticleSystem::~Dx11_ParticleSystem()
{
}


bool Dx11_ParticleSystem::Inittialize(ID3D11Device *pDevice, WCHAR *pTextureFile)
{
	//SET PARTICLE SYSTEM
	m_vDeviation = D3DXVECTOR3(3.0f, 0.1f, 2.0f);
	m_fParticleVelocity = 10.0f;
	m_fParticleVelocityVariation = 0.2f;
	m_fParticleSize = 0.05f;
	m_fParticleEmitionRate = 1000.0f;
	m_nMaxParticles = 50000;
	m_pParticleList = new stParticleType[m_nMaxParticles];
	for (unsigned int i = 0; i < m_nMaxParticles; i++)
	{
		m_pParticleList[i].life = false;
	}

	m_nCurrentParticleCount = 0;
	m_fTimeDuration = 0;

	if (InitializeBuffer(pDevice) == false)
		return false;

	if (m_pTexture->Initiazlize(pDevice, pTextureFile) == false)
	{
		MessageBox(NULL, L"Particle Texture Fail...", L"error", MB_ICONWARNING);
	}


	return true;
}


bool Dx11_ParticleSystem::InitializeBuffer(ID3D11Device *pDevice)
{
	
	m_nVertexCount = m_nMaxParticles*6;
	m_pVertices = new stPARTICLE_VERTEX[m_nVertexCount];
	memset(m_pVertices, 0, sizeof(stPARTICLE_VERTEX)*m_nVertexCount);


	m_nIndexCount = m_nMaxParticles * 6;
	unsigned int *pIndices = new unsigned int[m_nIndexCount];
	for (unsigned int i = 0; i < m_nIndexCount; i++)
	{		
		/*
		int index = 6 * i;
		pIndices[index + 0] = 4 * i;
		pIndices[index + 1] = 4 * i + 1;
		pIndices[index + 2] = 4 * i + 2;
		
		pIndices[index + 3] = 4 * i + 2;
		pIndices[index + 4] = 4 * i + 1;
		pIndices[index + 5] = 4 * i + 3;
		*/
		pIndices[i] = i;
	}


	D3D11_BUFFER_DESC desc, indxDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(stPARTICLE_VERTEX)* m_nVertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;


	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = m_pVertices;
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

	//delete[] m_pVertices;
	delete[] pIndices;


	return true;
}


void Dx11_ParticleSystem::Shutdown()
{

	if (m_pTexture)
	{
		m_pTexture->Shutdown();
		m_pTexture = NULL;
	}

	if (m_pVertices)
	{
		delete m_pVertices;
		m_pVertices = NULL;
	}

	if (m_pParticleList)
	{
		delete m_pParticleList;
		m_pParticleList = 0;
	}


	if (m_pIB)
	{
		m_pIB->Release();
		m_pIB = NULL;
	}

	if (m_pVB)
	{
		m_pVB->Release();
		m_pVB = 0;
	}
}



void Dx11_ParticleSystem::EmitParticles(float fTick)
{
	bool bToEmit = false;
	m_fTimeDuration += fTick;
	if (m_fTimeDuration > (1000 / m_fParticleEmitionRate))
	{
		m_fTimeDuration = 0.0f;
		bToEmit = true;
	}

	
	int nNumParticleAdd = 10;
	if (bToEmit && m_nCurrentParticleCount <= m_nMaxParticles - 1)
	{
		float posX, posY, posZ;
		posX = (((float)rand() - (float)rand()) / RAND_MAX) * m_vDeviation.x;
		posY = (((float)rand() - (float)rand()) / RAND_MAX) * m_vDeviation.y;
		posZ = 0;// (((float)rand() - (float)rand()) / RAND_MAX) * m_vDeviation.z;

		for (int kk = 0; ((kk < nNumParticleAdd) && (m_nCurrentParticleCount < m_nMaxParticles)); kk++)
		{
			m_nCurrentParticleCount++;		

			posY -= 0.5*kk;
			posX += (((float)rand() - (float)rand()) / RAND_MAX) * 0.1;
			float r, g, b;	//GENERATING YELLOW COLOR
			r = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
			g = r;// (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
			b = 0;// (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;


			unsigned int i = 0;
			bool bFound = false;
			/*for (i = 0; i < m_nMaxParticles; i++)
			{
			if (m_pParticleList[i].life == true)
			{
			if (m_pParticleList[i].vPosition.z < posZ)
			{
			bFound = true;
			break;
			}
			}
			}*/
			while (!bFound)
			{
				if (i < m_nMaxParticles)
				{
					if ((m_pParticleList[i].life == false) && (m_pParticleList[i].vPosition.z < posZ))
					{
						bFound = true;
					}
					else
					{
						i++;
					}
				}
				else
				{
					//INSERT IN LAST					
					m_nCurrentParticleCount--;
					return;
				}
			}


			unsigned int j = m_nCurrentParticleCount;
			unsigned int k = j - 1;
			while (j != i && k > 0)	//Updating Position of Previous Particle so that in Next Render they move downward
			{
				m_pParticleList[j].vPosition = m_pParticleList[k].vPosition;
				m_pParticleList[j].color = m_pParticleList[k].color;
				m_pParticleList[j].velocity = m_pParticleList[k].velocity;
				m_pParticleList[j].life = m_pParticleList[k].life;
				j--;
				k--;
			}

			m_pParticleList[i].vPosition = D3DXVECTOR3(posX, posY, posZ);
			m_pParticleList[i].color = D3DXCOLOR(r, g, b, 1.0f);
			m_pParticleList[i].velocity = m_fParticleVelocity + (((float)rand() - (float)rand()) / RAND_MAX)*m_fParticleVelocityVariation;
			m_pParticleList[i].life = true;
		}		
	}

}

void Dx11_ParticleSystem::UpdateParticles(float fTick)
{
	for (int i = 0; i < m_nCurrentParticleCount; i++)
	{
		m_pParticleList[i].vPosition.y -= m_pParticleList[i].velocity*fTick;
	}
}


void Dx11_ParticleSystem::KillParticles()
{
	for (unsigned int i = 0; i < m_nMaxParticles; i++)
	{
		if (m_pParticleList[i].life && (m_pParticleList[i].vPosition.y < -100.0f))
		{
			m_pParticleList[i].life = false;
			m_nCurrentParticleCount--;

			for (unsigned int j = i; j < m_nMaxParticles - 1; j++)
			{
				m_pParticleList[j].vPosition = m_pParticleList[j+1].vPosition;
				m_pParticleList[j].color = m_pParticleList[j + 1].color;
				m_pParticleList[j].velocity = m_pParticleList[j + 1].velocity;
				m_pParticleList[j].life = m_pParticleList[j + 1].life;
			}			
		}		
	}

}


void Dx11_ParticleSystem::UpdateBuffer(ID3D11DeviceContext *pDeviceContext)
{

	int index = 0;
	memset(m_pVertices, 0, sizeof(stPARTICLE_VERTEX)*m_nVertexCount);

	for (int i = 0; i < m_nCurrentParticleCount; i++)
	{
		
		D3DXVECTOR3	p1, p2, p3, p4;
		float fR, fG, fB;

		p1 = D3DXVECTOR3(m_pParticleList[i].vPosition.x - m_fParticleSize, m_pParticleList[i].vPosition.y + m_fParticleSize, m_pParticleList[i].vPosition.z);
		p2 = D3DXVECTOR3(m_pParticleList[i].vPosition.x + m_fParticleSize, m_pParticleList[i].vPosition.y + m_fParticleSize, m_pParticleList[i].vPosition.z);
		p3 = D3DXVECTOR3(m_pParticleList[i].vPosition.x - m_fParticleSize, m_pParticleList[i].vPosition.y - m_fParticleSize, m_pParticleList[i].vPosition.z);
		p4 = D3DXVECTOR3(m_pParticleList[i].vPosition.x + m_fParticleSize, m_pParticleList[i].vPosition.y - m_fParticleSize, m_pParticleList[i].vPosition.z);

		fR = m_pParticleList[i].color.r;
		fG = m_pParticleList[i].color.g;
		fB = m_pParticleList[i].color.b;


		m_pVertices[index].vPos = p1;
		m_pVertices[index].diffuse = D3DXVECTOR4(fR, fG, fB, 1.0f);
		m_pVertices[index].uv = D3DXVECTOR2(0.0f, 0.0f);
		index++;

		m_pVertices[index].vPos = p2;
		m_pVertices[index].diffuse = D3DXVECTOR4(fR, fG, fB, 1.0f);
		m_pVertices[index].uv = D3DXVECTOR2(1.0f, 0.0f);
		index++;
		
		m_pVertices[index].vPos = p3;
		m_pVertices[index].diffuse = D3DXVECTOR4(fR, fG, fB, 1.0f);
		m_pVertices[index].uv = D3DXVECTOR2(0.0f, 1.0f);
		index++;


		m_pVertices[index].vPos = p3;
		m_pVertices[index].diffuse = D3DXVECTOR4(fR, fG, fB, 1.0f);
		m_pVertices[index].uv = D3DXVECTOR2(0.0f, 1.0f);
		index++;

		
		m_pVertices[index].vPos = p2;
		m_pVertices[index].diffuse = D3DXVECTOR4(fR, fG, fB, 1.0f);
		m_pVertices[index].uv = D3DXVECTOR2(1.0f, 0.0f);
		index++;


		m_pVertices[index].vPos = p4;
		m_pVertices[index].diffuse = D3DXVECTOR4(fR, fG, fB, 1.0f);
		m_pVertices[index].uv = D3DXVECTOR2(1.0f, 1.0f);
		index++;

	}

	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return ;

	stPARTICLE_VERTEX *pVertices = (stPARTICLE_VERTEX *)mappedResource.pData;
	memcpy(pVertices, m_pVertices, sizeof(stPARTICLE_VERTEX)*index);
	pDeviceContext->Unmap(m_pVB, 0);


}



void Dx11_ParticleSystem::Prcoess(ID3D11DeviceContext *pDeviceContext, float _fTick)
{

	KillParticles();
	EmitParticles(_fTick);
	UpdateParticles(_fTick);
	UpdateBuffer(pDeviceContext);

}


void Dx11_ParticleSystem::Render(ID3D11DeviceContext *pDeviceContext)
{
	if (pDeviceContext)
	{
		UINT stride = sizeof(stPARTICLE_VERTEX);
		UINT offset = 0;

		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

}













